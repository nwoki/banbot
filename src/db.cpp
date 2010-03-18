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



#include "db.h"
#include "logger.h"

//Db::Db( Logger * log, vector<ConfigLoader::Option> conf ):
Db::Db( vector<ConfigLoader::Option> conf, vector<ConfigLoader::Banlist> banned, Logger* log ):
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
    setupAdmins( conf );    //TODO make "loadAdmins" without deleting and re writing db
    loadBanlist( banned );
    dumpBanned();

    //chiudo il log.
    logger->close();
}

Db::~Db()
{
}

void Db::setupAdmins( vector<ConfigLoader::Option> opzioni )
{
    cout<<"  [-] setting up admin guid's.. \n";
    *logger<<"  [-] setting up admin guid's.. \n";

    string clearQuery("delete from oplist;");

    if( resultQuery( clearQuery ) == 0 ){
        cout<<"    [*]cleaned admin table..\n";
        *logger<<"    [*]cleaned admin table..\n";
    }
    else{
        cout<<"\e[1;31m    [EPIC FAIL] Db::setupAdmins can't execute query!\e[0m \n";
        *logger<<"    [EPIC FAIL] Db::setupAdmins can't execute query!\n";
    }

    cout<<"    [-]trying to repopulate database\n";
    *logger<<"    [-]trying to repopulate database\n";

    for( int i = 0; i < opzioni.size(); i++ ){
        if( opzioni[i].name.compare("GUID") == 0 ){
            string aux;
            aux.append("insert into oplist values('");
            aux.append(opzioni[i].value);
            aux.append("');");;

            if ( resultQuery( aux ) == 0 ){
                cout<<"\e[0;32m      [+]added admin: " << opzioni[i].value << "\e[0m \n";
                *logger<<"      [+]added admin: " << opzioni[i].value << "\n";
            }
            else{
                cout<<"\e[0;31m      [FAIL] can't add admin to database!\e[0m \n";
                *logger<<"      [FAIL] can't add admin to database!\n";
            }
        }
    }
}

void Db::loadBanlist( vector<ConfigLoader::Banlist> banned )
{
    if( banned.size() == 0 ){
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
    cout << "\e[0;32m[*] Added [ " << addedCounter << "/" << banned.size() << " ] guids from file to database banlist\e[0m \n";
}


bool Db::connect()
{
    if(sqlite3_open( DATABASE, &database )){
        cout<<"\e[0;31m[FAIL] " << sqlite3_errmsg( database ) << "\e[0m \n";
        *logger<<"[FAIL] " << sqlite3_errmsg( database );
        close();
        return false;
    }
    else return true;
}

void Db::createDb()   //initial creation of database
{
    //to dump db from terminal
    //sqlite3  Db.sqlite ".dump" > test

    //create tables, oplist(nick, guid) and banned(id(autoincrement),ip,date,time),nick(id,nick), guid(id,guid)
    string createBannedTable(
    "create table banned("
    "id INTGER PRIMARY KEY,"    //autoincrement
    "ip TEXT,"
    "date TEXT,"
    "time TEXT,"
    "FOREIGN KEY( id ) REFERENCES nick( id ) );" );

    string createNickTable(
    "create table nick("
    "id INTEGER PRIMARY KEY,"
    "name TEXT );" );

    string createGuidTable(
    "create table guids("
    "id INTEGER PRIMARY KEY,"
    "guid TEXT,"
    "FOREIGN KEY( id ) REFERENCES nick( id ) );" );

    //checks...
    if( resultQuery( createBannedTable ) == 0 ){
        cout<<"\e[0;32m    [*]created 'banned' table..\e[0m \n";
        *logger<<"    [*]created 'banned' table..\n";
    }
    else{
        cout<<"\e[0;31m    [FAIL]error creating 'banned' table\e[0m \n";
        *logger<<"    [FAIL]error creating 'banned' table\n";
    }

    if( resultQuery( createNickTable ) == 0 ){
        cout<<"\e[0;32m    [*]created 'nick' table..\e[0m \n";
        *logger<<"    [*]created 'nick' table..\n";
    }
    else{
        cout<<"\e[0;31m    [FAIL]error creating 'nick' table\e[0m \n";
        *logger<<"    [FAIL]error creating 'nick' table\n";
    }

    if( resultQuery( createGuidTable ) == 0 ){
        cout<<"\e[0;32m     [*]created 'guid' table..\e[0m \n";
        *logger<<"    [*]created 'guid' table..\n";
    }
    else{
        cout<<"\e[0;31m    [FAIL]error creating 'guid' table\e[0m \n";
        *logger<<"    [FAIL]error creating 'guid' table\n";
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

    cout<<"\n\n[-]Trying to dump banned guid's..\n";
    *logger<<"[-]Trying to dump banned guid's..\n";

    if( !resultQuery( query ) ){
        cout<<"\e[0;33m[!]no banned guids to dump to file\e[0m \n";
        *logger<<"[!]no banned guids to dump to file\n";
        return;
    }

    //ready to dump to file
    std::vector<std::string> dataToDump = extractData( query );

    //check for old file. If exists, delete it
    struct stat st;

    if( stat( "backup/Banlist.backup", &st ) == 0 ){
        std::cout << "[-]deleting old banlist dump file..\n";
        if ( !system( "rm backup/Banlist.backup" ) )
            cout<< "\e[0;33m[!]deleted old backup file..creating new one..\e[0m \n";
        else cout<< "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
    }

    //use logger class to save info to file
    Logger *output = new Logger( "backup/Banlist.backup" );

    if ( !output->open() ){
        cout<<"\e[0;31m[ERR] can't write Banlist backup file!\e[0m \n";
        *logger<<"[ERR] can't write Banlist backup file!\n";
        return;
    }

    for( int i = 0; i < dataToDump.size(); i++ )
        *output<<dataToDump[i]<<"\n";

    cout<<"\e[0;32m[OK] successfully written Banlist.backup file in 'backup/'\e[0m \n";
    *logger<<"[OK] successfully written Banlist.backup file in 'backup/'\n";

    output->close();

    delete output;  //non mi serve più
}

std::vector<std::string> Db::extractData( const string &query )    //returns vector with results as string
{
    SQLITE3 data( DATABASE );

    if( !data.exe( query ) )  //found data
        return data.vdata; //get it
}


int Db::resultQuery( const string &a ) //ritorna quante corrispondenze ci sono all'interno del DB
{
    int answer = 0;
    SQLITE3 *query = new SQLITE3( DATABASE );
    //eseguo la query: se fallisce (ritorna true) imposto la risposta a -1
    if ( query->exe( a ) ) answer = -1;
    //altrimenti restituisco la dimensione dell'array contenente i risultati (non mi interessa avere i risultati)
    else answer = query->vdata.size();

    delete ( query );
    return answer;
}

bool Db::ban( const string &guid ) //adds banned guid to database
{
    //add
    string aux( "insert into banned values('" );
    aux.append( guid );
    aux.append( "');" );

    if( resultQuery( aux ) == 0 ){
        cout<<"\e[0;32m[OK] ban applied on: "<<guid<<"\e[0m \n";
        *logger<<"[OK] ban applied on "<<guid<<"\n";
        return true;
    }
    else{
        cout<<"\e[0;31m[FAIL] ban not applied on: "<<guid<<"\e[0m \n";
        *logger<<"[FAIL] ban not applied on "<<guid<<"\n";
        return false;
    }
}

void Db::close()
{
    sqlite3_close( database );
}
