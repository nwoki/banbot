/*
    db.cpp is part of BanBot.

    BanBot is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BanBot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BanBot (look at GPL_License.txt).
    If not, see <http://www.gnu.org/licenses/>.

    Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/


#include <sstream>  //per conversione int a str
#include "db.h"
#include "logger.h"

//Db::Db( Logger * log, vector<ConfigLoader::Option> conf ):
Db::Db( vector<ConfigLoader::Option> conf, vector<ConfigLoader::Banlist> banned, vector<ConfigLoader::Option> admins, Logger* log ):
    logger( log )
{
    //effettuo il logging del check up del database:
    logger->open();
    //check della directory database
    struct stat st;

    string path( DATABASE );
    size_t pos = path.find_last_of( '/' );
    string cartella = path.substr( 0, pos );
    string file = path.substr( pos+1 );

    cout<<"[*]Starting up...\n";
    cout<<"[-]checking for database directory..\n";
    *logger<<"\n**************************************************************************************\n";
    logger->timestamp();
    *logger<<"\n[*]Starting up...\n";
    *logger<<"[-]checking for database directory..\n";

    if( stat( cartella.c_str(), &st ) == 0 ){
        cout<<"  [*]dir '"<<cartella<<"/' found\n";
        *logger<<"  [*]dir '"<<cartella<<"/' found\n";
    }
    else{
        cout<<"\e[0;33m  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\e[0m \n";
        *logger<<"  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";

        if( mkdir( cartella.c_str(), 0777 ) != 0 ){
            cout<<"\e[1;31m[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\e[0m \n";
            *logger<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
            cout<<"\e[1;31m[FAIL] need database for bot to work correctly. Please resolve this problem and launch application again\e[0m \n";
            *logger<<"[FAIL] can't create database! Haven't got permission to do so.. TERMINATING BOT!\n";
            //TODO! terminate program here!!!

        }
        else{
            cout<<"\e[0;32m  [OK]created '"<<cartella<<"/' directory..\e[0m \n";
            *logger<<"  [OK]created '"<<cartella<<"/' directory..\n";
        }
    }

    //check per il database
    ifstream IN( DATABASE );

    if( !IN ){
        cout<<"\e[0;33m  [!] database doesn't exist!\e[0m \n";
        //create database
        cout<<"\e[0;33m    [!] creating database '"<<file<<"/' in '"<<cartella<<"/'\e[0m \n";
        *logger<<"  [!] database doesn't exist!\n"<<"    [*] creating database '"<<file<<"/' in '"<<cartella<<"/'\n";
        createDb();
    }
    IN.close();

    //azzero la tabella degli admins
    setupAdmins( admins );    //TODO make "loadAdmins" without deleting and re writing db
    loadBanlist( banned );
    dumpBanned();

    //chiudo il log.
    logger->close();
}


Db::~Db()
{
}


void Db::setupAdmins( vector<ConfigLoader::Option> admins )
{
    cout<<"  [-] setting up admin guid's.. \n";
    *logger<<"  [-] setting up admin guid's.. \n";

    string clearQuery( "delete from oplist;" );

    if( execQuery( clearQuery ) ){
        cout<<"    [*]cleaned admin table..\n";
        *logger<<"    [*]cleaned admin table..\n";
    }
    else{
        cout<<"\e[1;31m    [EPIC FAIL] Db::setupAdmins can't clean table\e[0m \n";
        *logger<<"    [EPIC FAIL] Db::setupAdmins can't clean table \n";
    }

    cout<<"    [-]trying to repopulate database\n";
    *logger<<"    [-]trying to repopulate database\n";

    for( unsigned int i = 0; i < admins.size(); i++ ){
        if( addOp( admins[i].name, admins[i].value ) ){
            cout<<"\e[0;32m      [+]added admin: " << admins[i].name << "\e[0m \n";//opzioni[i].value << "\e[0m \n";
            *logger<<"      [+]added admin: " << admins[i].value << "\n";
        }
        else{
            cout<<"\e[0;31m      [FAIL] can't add admin to database!\e[0m \n";
            *logger<<"      [FAIL] can't add admin to database!\n";
        }
    }
}


void Db::loadBanlist( vector<ConfigLoader::Banlist> banned )
{
    /*if( banned.size() == 0 ){
        cout<<"\e[0;33m[!]Banlist EMPTY\e[0m \n";
        *logger<<"[!]Banlist EMPTY\n";
        return;
    }

    int addedCounter = 0, onDbCounter = 0;
    //adding banned users to db
    cout<<"\n[-]Adding banned users to database..\n\n";

    for( int i = 0; i < banned.size(); i++ ){
        if( checkBanGuid( banned[i].banGuid ) == 0 ){    //add if not on db
            if ( ban( banned[i].banGuid ) )   //if success, add 1 to counter
                addedCounter++;
        }
        else onDbCounter++;
    }

    cout << "\n\n\e[0;33m[!] " << onDbCounter << " already on database\e[0m \n";
    cout << "\e[0;32m[*] Added [ " << addedCounter << "/" << banned.size() << " ] guids from file to database banlist\e[0m \n";*/
}


bool Db::connect()
{
    if( sqlite3_open( DATABASE, &database )){
        cout<<"\e[0;31m[FAIL] " << sqlite3_errmsg( database ) << "\e[0m \n";
        *logger<<"[FAIL] " << sqlite3_errmsg( database );
        close();
        return false;
    }
    else return true;
}


void Db::createDb()   //initial creation of database
{
    string createBannedTable(
    "create table banned("
    "id INTEGER PRIMARY KEY,"    //autoincrement
    "nick TEXT,"
    "ip TEXT,"
    "date TEXT,"
    "time TEXT,"
    "motive TEXT);" );

    string createGuidTable( //for banned users
    "create table guids("
    "id INTEGER PRIMARY KEY,"    //autoincrement
    "guid TEXT,"
    "banId TEXT,"
    "FOREIGN KEY( banId ) REFERENCES banned( id ) );" );

    string createOplistTable(
    "create table oplist("
    "id INTEGER PRIMARY KEY,"
    "nick TEXT,"
    "guid TEXT);" );

    //checks...
    if( resultQuery( createBannedTable ) == 0 ){
        cout<<"\e[0;32m    [*]created 'banned' table..\e[0m \n";
        *logger<<"    [*]created 'banned' table..\n";
    }
    else{
        cout<<"\e[0;31m    [FAIL]error creating 'banned' table\e[0m \n";
        *logger<<"    [FAIL]error creating 'banned' table\n";
    }

    if( resultQuery( createGuidTable ) == 0 ){
        cout<<"\e[0;32m     [*]created 'guid' table..\e[0m \n";
        *logger<<"    [*]created 'guid' table..\n";
    }
    else{
        cout<<"\e[0;31m    [FAIL]error creating 'guid' table\e[0m \n";
        *logger<<"    [FAIL]error creating 'guid' table\n";
    }

    if( resultQuery( createOplistTable ) == 0 ){
        cout<<"\e[0;32m     [*]created 'oplist' table..\e[0m \n";
        *logger<<"    [*]created 'oplist' table..\n";
    }
    else{
        cout<<"\e[0;31m    [FAIL]error creating 'oplist' table\e[0m \n";
        *logger<<"    [FAIL]error creating 'oplist' table\n";
    }

}


//checks oplist for ops
bool Db::checkAuthGuid( const string &guid )
{
    string aux( "select guid from oplist where guid='" );
    aux.append( guid );
    aux.append( "'" );

    return ( resultQuery( aux ) > 0 );
}


bool Db::checkBanGuid( const string &banGuid )
{
    string aux( "select guid from banned where guid='" );
    aux.append( banGuid );
    aux.append( "'" );

    cout << "[-] Test sul ban " << banGuid << "\n";
    *logger << "[-] Test sul ban "<< banGuid << "\n";

    return ( resultQuery( aux ) > 0 );
}


void Db::dumpBanned()
{
    string query( "select *from banned" );

    cout << "\n\n[-]Trying to dump banned guid's..\n";
    *logger << "[-]Trying to dump banned guid's..\n";

    if( !resultQuery( query ) ){
        cout << "\e[0;33m[!]no banned guids to dump to file\e[0m \n";
        *logger << "[!]no banned guids to dump to file\n";
        return;
    }

    //ready to dump to file
    vector< string > dataToDump = extractData( query );

    //check for old file. If exists, delete it
    struct stat st;

    if( stat( "backup/Banlist.backup", &st ) == 0 ){
        cout << "[-]deleting old banlist dump file..\n";
        if ( !system( "rm backup/Banlist.backup" ) )
            cout << "\e[0;33m[!]deleted old backup file..creating new one..\e[0m \n";
        else cout << "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
    }

    //use logger class to save info to file
    Logger *output = new Logger( "backup/Banlist.backup" );

    if ( !output->open() ){
        cout << "\e[0;31m[ERR] can't write Banlist backup file!\e[0m \n";
        *logger << "[ERR] can't write Banlist backup file!\n";
        return;
    }

    for( unsigned int i = 0; i < dataToDump.size(); i++ )
        *output << dataToDump[i] << "\n";

    cout << "\e[0;32m[OK] successfully written Banlist.backup file in 'backup/'\e[0m \n";
    *logger << "[OK] successfully written Banlist.backup file in 'backup/'\n";

    output->close();

    delete output;  //non mi serve più
}


void Db::dumpDatabase() //to finish and understand
//http://www.sqlite.org/backup.html
{
    //look at link
}


vector< string > Db::extractData( const string &query )    //returns vector with results as string
{
    SQLITE3 data( DATABASE );

    if( !data.exe( query ) )  //found data
        return data.vdata; //get it
    else {
        vector< string > asd; //else return empty vector
        return asd;
    }
}


int Db::resultQuery( const string &query ) //ritorna quante corrispondenze ci sono all'interno del DB
{
    int answer = 0;
    SQLITE3 *sql = new SQLITE3( DATABASE );
    //eseguo la query: se fallisce (ritorna true) imposto la risposta a -1
    if ( sql->exe( query ) ) answer = -1;
    //altrimenti restituisco la dimensione dell'array contenente i risultati (non mi interessa avere i risultati)
    else answer = sql->vdata.size();

    delete ( sql );
    return answer;
}


bool Db::execQuery( const string &query )
{
    SQLITE3 *sql = new SQLITE3( DATABASE );
    bool success;

    if( !sql->exe( query ) )
        success = true;    //SUCCESS
    else{
        success = false;  //FAIL
        cout << "\e[1;31m[EPIC FAIL] Db::execQuery: " << query << " \e[0m \n";
        *logger << "[EPIC FAIL] Db::execQuery: " << query << "\n";
    }

    delete( sql );
    return success;
}

bool Db::ban( const string &ip, const string &nick, const string &date, const string &time, const string &guid, const string &motive ) //adds banned guid to database
{
    string banId = insertNewBanned( nick, ip, date, time, motive );    //get the autoincrement id

    if( banId.empty() )    // empty string is ERROR
        return false;    //didn't write to database

    if( insertNewGuid( guid, banId ).empty() )    // empty string is ERROR
        return false;

    return true;
    //else went well

    /*if( resultQuery( aux ) == 0 ){
        cout<<"\e[0;32m[OK] ban applied on: "<<guid<<"\e[0m \n";
        *logger<<"[OK] ban applied on "<<guid<<"\n";
        return true;
    }
    else{
        cout<<"\e[0;31m[FAIL] ban not applied on: "<<guid<<"\e[0m \n";
        *logger<<"[FAIL] ban not applied on "<<guid<<"\n";
        return false;
    }*/
}


//BANTABLE METHODS
string Db::insertNewBanned( const string& nick, const string& ip, const string& date, const string &time, const string &motive )
{
    string newBanQuery( "insert into banned( nick, ip, date, time, motive ) values('" );

    newBanQuery.append( nick );
    newBanQuery.append( "','" );
    newBanQuery.append( ip );
    newBanQuery.append( "','" );
    newBanQuery.append( date );
    newBanQuery.append( "','" );
    newBanQuery.append( time );
    newBanQuery.append( "','" );
    newBanQuery.append( motive );
    newBanQuery.append( "');" );

    if( !execQuery( newBanQuery ) )
        return string();  //FAIL return empty string

    //else return last banId
    vector< string > max;

    max = extractData( "select max( id ) from banned;" );

    if( max.empty() )
        return string();
    else return max[0];
}


bool Db::modifyBanned( const string &nick, const string &ip, const string &date, const string &time, const string &motive, const string &id )
{
    string query( "update banned set " );
    bool paramCount = false;
    //TODO semplificare questa funzione usando un vector o struct con chiave-valore

    if( !nick.empty() ){
        query.append( "nick = '" );
        query.append( nick );
        query.append( "' " );
        paramCount = true;
    }

    if( !ip.empty() ){
        if( paramCount )
            query.append( "," );
        query.append( "ip = '" );
        query.append( ip );
        query.append( "' " );
        paramCount = true;
    }

    if( !date.empty() ){
        if( paramCount )
            query.append( "," );
        query.append( "date = '" );
        query.append( date );
        query.append( "' " );
        paramCount = true;
    }

    if( !time.empty() ){
        if( paramCount )
            query.append( "," );
        query.append( "time = '" );
        query.append( time );
        query.append( "' " );
    }

    if( !motive.empty() ){
        if( paramCount )
            query.append( "," );
        query.append( "motive = '" );
        query.append( motive );
        query.append( "' " );
    }

    query.append( "where id = '" );
    query.append( id );
    query.append( "';" );

    //cout << "\nmodify banned" << query << "\n"
    ;
    if( !execQuery( query ) ){
        cout << "\e[0;31m[FAIL] Db::modifyBanned : " << query << "\e[0m \n";
        *logger << "[FAIL] Db::modifyBanned : " << query << "\n";
        return false;
    }
    else return true;
}


bool Db::deleteBanned( const string &id )
{
    string query( "delete from banned where id ='" );
    query.append( id );
    query.append( "';" );

    if( !execQuery( query ) ){
        cout << "\e[0;31m[FAIL] Db::deleteBanned : " << query << "\e[0m \n";
        *logger << "[FAIL] Db::deleteBaned : " << query << "\n";
        return false;
    }
    else return true;
}


//GUID TABLE METHODS
string Db::insertNewGuid( const string& guid, const string &banId )
{
    string newGuidQuery( "insert into guids( guid, banId ) values('" );

    newGuidQuery.append( guid );
    newGuidQuery.append( "','" );
    newGuidQuery.append( banId );
    newGuidQuery.append( "');" );

    if( !execQuery( newGuidQuery ) )
        return string();  //FAIL return empty string

    //else return last banId
    vector< string > max;

    max = extractData( "select max( id ) from guids" );
    return max[0];
}


bool Db::modifyGuid( const string& guid, const string &banId, const string &id )
{
    string query( "update guids set ") ;
    bool paramCount = false;

    if( !guid.empty() ){
        query.append( "guid = '" );
        query.append( guid );
        query.append( "' " );
        paramCount = true;
    }

    if( !banId.empty() ){
        if( paramCount )
            query.append( "," );
        query.append( "banId = '" );
        query.append( banId );
        query.append( "' " );
    }

    query.append( "where id = '" );
    query.append( id );
    query.append( "';" );

    if( !execQuery( query ) ){
        cout << "\e[0;31m[FAIL] Db::modifyGuid : " << query << "\e[0m \n";
        *logger << "[FAIL] Db::modifyGuid : " << query << "\n";
        return false;
    }
    else return true;
}


bool Db::deleteGuid( const string &id )
{
    string query( "delete from guids where id ='" );
    query.append( id );
    query.append( "';" );

    if( !execQuery( query ) ){
        cout << "\e[0;31m[FAIL] Db::deleteGuid : " << query << "\e[0m \n";
        *logger << "[FAIL] Db::deleteGuid : " << query << "\n";
        return false;
    }
    else return true;
}


//OPLIST TABLE METHODS
bool Db::addOp( const string& nick, const string& guid )
{
    string addOpQuery( "insert into oplist( nick, guid ) values('" );
    addOpQuery.append( nick );
    addOpQuery.append( "','" );
    addOpQuery.append( guid );
    addOpQuery.append( "');" );

    return execQuery( addOpQuery ); //true on success
}


bool Db::modifyOp( const string& nick, const string& guid, const string& id )
{
    string modifyQuery( "update oplist set ") ;
    bool paramCount = false;

    if( !nick.empty() ){
        modifyQuery.append( "nick = '" );
        modifyQuery.append( nick );
        modifyQuery.append( "' " );
        paramCount = true;
    }

    if( !guid.empty() ){
        if( paramCount )
            modifyQuery.append( "," );
        modifyQuery.append( "guid = '" );
        modifyQuery.append( guid );
        modifyQuery.append( "' " );
    }

    modifyQuery.append( "where id = '" );
    modifyQuery.append( id );
    modifyQuery.append( "';" );

    if( !execQuery( modifyQuery ) ){
        cout << "\e[0;31m[FAIL] Db::modifyOp : " << modifyQuery << "\e[0m \n";
        *logger << "[FAIL] Db::modifyOp : " << modifyQuery << "\n";
        return false;
    }
    else return true;
}


bool Db::deleteOp( const string& id )
{
    string deleteQuery( "delete from oplist where id='" );
    deleteQuery.append( id );
    deleteQuery.append( "';" );

    return execQuery( deleteQuery );
}


string Db::intToString( int number )
{
    stringstream out;//barbatrucco ;)

    out << number;
    return out.str();
}


void Db::close()
{
    sqlite3_close( database );
}
