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
        cout<<"  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";
        *logger<<"  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";

        if( mkdir( cartella.c_str(), 0777 ) != 0 ){
            cout<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
            *logger<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
            cout<<"[FAIL] need database for bot to work correctly. Please resolve this problem and launch application again\n";
            *logger<<"[FAIL] can't create database! Haven't got permission to do so.. TERMINATING BOT!\n";
            //TODO! terminate program here!!!

        }
        else{
            cout<<"  [OK]created '"<<cartella<<"/' directory..\n";
            *logger<<"  [OK]created '"<<cartella<<"/' directory..\n";
        }
    }

    //check per il database
    ifstream IN( DATABASE );

    if( !IN ){
        cout<<"  [!] database doesn't exist!\n";
        //create database
        cout<<"    [*] creating database '"<<file<<"/' in '"<<cartella<<"/'\n";
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
        cout<<"    [EPIC FAIL] Db::setupAdmins can't execute query!\n";
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
                cout<<"      [+]added admin: " << opzioni[i].value << "\n";
                *logger<<"      [+]added admin: " << opzioni[i].value << "\n";
            }
            else{
                cout<<"      [FAIL] can't add admin to database!\n";
                *logger<<"      [FAIL] can't add admin to database!\n";
            }
        }
    }
}

void Db::loadBanlist( vector<ConfigLoader::Banlist> banned )
{
    if( banned.size() == 0 ){
        cout<<"[!]Banlist EMPTY\n";
        *logger<<"[!]Banlist EMPTY\n";
        return;
    }

    int addedCounter = 0, onDbCounter = 0;
    //adding banned users to db
    cout<<"\n[-]Adding banned users to database..\n\n";

    for( int i = 0; i < banned.size(); i++ ){
        if( !checkBanGuid( banned[i].banGuid ) ){    //add if not on db
            ban( banned[i].banGuid );   //if success, add 1 to counter
            addedCounter++;
        }
        else onDbCounter++;
    }

    cout << "\n\n[!] " << onDbCounter << " already on database\n";
    cout << "[*] Added [ " << addedCounter << "/" << banned.size() << " ] guids from file to database banlist\n";
}


bool Db::connect()
{
    if(sqlite3_open( DATABASE, &database )){
        cout<<"[FAIL] " << sqlite3_errmsg( database );
        *logger<<"[FAIL] " << sqlite3_errmsg( database );
        close();
        return false;
    }
    else return true;
}

void Db::createDb()   //initial creation of database
{
    //create tables, oplist(nick, guid) and banned(guid)
    string queryStr("create table banned(guid text)");

    if( resultQuery( queryStr ) == 0 ){
        cout<<"    [*]created banned table..\n";
        *logger<<"    [*]created banned table..\n";
    }
    else{
        cout<<"    [FAIL]error creating banned table\n";
        *logger<<"    [FAIL]error creating banned table\n";
    }

    //oplist
    queryStr.clear();
    queryStr.append("create table oplist(guid text)");
    if ( resultQuery( queryStr ) == 0 ){
        cout<<"    [*]created oplist table..\n";
        *logger<<"    [*]created oplist table..\n";
    }
    else{
        cout<<"    [FAIL]error creating oplist table\n";
        *logger<<"    [FAIL]error creating oplist table\n";
    }
}

//checks oplist for ops
bool Db::checkAuthGuid( string guid )
{
    string aux( "select guid from oplist where guid='" );
    aux.append( guid );
    aux.append( "'" );

    return ( resultQuery( aux ) > 0 );
}

bool Db::checkBanGuid( string banGuid )
{
    string aux( "select guid from banned where guid='" );
    aux.append( banGuid );
    aux.append( "'" );

    cout << "[-] Test sul ban " << banGuid << "\n";
    *logger << "[-] Test sul ban "<< banGuid << "\n";

    return ( resultQuery( aux ) > 0 );
}

/*bool Db::checkDirAndFile( string path ) //use to check dir and file. If not there, create them/it
{
    struct stat st;

    bool checkStatus = false;

    size_t pos = path.find_last_of( '/' );
    string cartella = path.substr( 0, pos );
    string file = path.substr( pos + 1 );

    if( stat( cartella.c_str(), &st ) == 0 ){
        cout<<"  [*]dir '"<<cartella<<"/' found\n";
        //*logger<<"  [*]dir '"<<cartella<<"/' found\n";
        checkStatus = true;
    }
    else{
        cout<<"  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";
        //*logger<<"  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";

        if( !mkdir( cartella.c_str(), 0777 )){
            cout<<"  [OK]created '"<<cartella<<"/' directory..\n";
            //*logger<<"  [OK]created '"<<cartella<<"/' directory..\n";
            checkStatus = true;
        }
        else{
            cout<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
            //*logger<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
            checkStatus = false;
        }
    }

    //check per il database
    //ifstream IN( path.c_str() );
    ofstream OUT( path.c_str() );

    if( OUT.is_open() ){
        OUT<<"TEST";
        OUT.close();
    }

    return checkStatus; //true if both dir and file are there
}*/

void Db::dumpBanned()
{
    string query( "select *from banned" );

    cout<<"\n\n[-]Trying to dump banned guid's..\n";
    *logger<<"[-]Trying to dump banned guid's..\n";

    if( !resultQuery( query ) ){
        cout<<"[!]no banned guids to dump to file";
        *logger<<"[!]no banned guids to dump to file";
        return;
    }

    //ready to dump to file
    std::vector<std::string> dataToDump = extractData( query );

    //check for old file. If exists, delete it
    struct stat st;

    if( stat( "backup/Banlist.backup", &st ) == 0 ){
        std::cout << "[-]deleting old banlist dump file..\n";
        if ( !system( "rm backup/Banlist.backup" ) )
            cout<< "[!]deleted old backup file..creating new one..\n";
        else cout<< "[ERR]couldn't delete old dump file!";
    }

    //use logger class to save info to file
    Logger *output = new Logger( "backup/Banlist.backup" );

    if ( !output->open() ){
        cout<<"[ERR] can't write Banlist backup file!\n";
        *logger<<"[ERR] can't write Banlist backup file!\n";
        return;
    }

    for( int i = 0; i < dataToDump.size(); i++ )
        *output<<dataToDump[i]<<"\n";

    cout<<"[OK] successfully written Banlist.backup file in 'backup/'\n";
    *logger<<"[OK] successfully written Banlist.backup file in 'backup/'\n";

    output->close();

    delete output;  //non mi serve più
}

std::vector<std::string> Db::extractData( string query )    //returns vector with results as string
{
    SQLITE3 data( DATABASE );

    if( !data.exe( query ) )  //found data
        return data.vdata; //get it
}


int Db::resultQuery( string a ) //ritorna quante corrispondenze ci sono all'interno del DB
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

void Db::ban( string guid ) //adds banned guid to database
{
    //add
    string aux( "insert into banned values('" );
    aux.append( guid );
    aux.append( "');" );

    if( resultQuery( aux ) == 0 ){
        cout<<"[OK] ban applied on "<<guid<<"\n";
        *logger<<"[OK] ban applied on "<<guid<<"\n";
    }
    else{
        cout<<"[FAIL] ban not applied on "<<guid<<"\n";
        *logger<<"[FAIL] ban not applied on "<<guid<<"\n";
    }
}

void Db::close()
{
    sqlite3_close( database );
}
