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


//#include <sstream>  //per conversione int a str
//#include <fstream>
//#include <sys/stat.h>
#include <stdlib.h>
#include "ConfigLoader.h"
#include "db.h"
#include "logger.h"
#include "handyFunctions.cpp"

/********************************
*       PUBLIC  METHODS         *
********************************/

Db::Db( ConfigLoader::Options* conf )
    : m_options( conf )
    , m_zErrMsg( 0 )
    , m_result( 0 )
{
    cout << "[*]Starting up...\n";
    cout << "[-]checking for database directory..\n";
    *(m_options->log) << "\n**************************************************************************************\n";
    m_options->log->timestamp();
    *(m_options->log) << "\n[*]Starting up...\n";
    *(m_options->log) << "[-]checking for database directory..\n";

    #ifdef DB_DEBUG
    //config output
    cout << "SETTINGS ***** \n";
    cout << m_options->toString();

    #endif

    //  check only if i have valid info loaded
    if( m_options->servers.size() > 0 ) {
        //create databases if needed
        for( unsigned int i = 0; i < m_options->servers.size(); i++ ) {
            //get path to db folder
            string dbPath = m_options->servers.at( i )->dbFolder();

            if( dbPath[dbPath.size()-1] != '/' )
                dbPath.append( "/" );

            string pathWithFile( dbPath ); //set full file path
            pathWithFile.append( DB_NAME );

            if( !fileExistance( dbPath ) ) {    //does directory exist?
                if( !dirCreate( dbPath ) ) {
                    cout << "\e[1;31mDb::Db couldn't create database folder: " << dbPath << "\e[0m \n";
                    *(m_options->log) << "Db::Db couldn't create database folder: " << dbPath << "\n";
                }
                else {
                    cout << "\e[0;32m  [OK]created " << dbPath << " directory..\e[0m \n";
                    *(m_options->log) << "  [OK]created " << dbPath << " directory..\n";
                }
            }
            else
                cout << "\e[0;33m database folder: " << dbPath << " already exists \e[0m \n";

            if( !fileExistance( pathWithFile ) ) {  //does file exist?
                if( !fileCreate( pathWithFile ) ) {
                    cout << "\e[1;31mDb::Db couldn't create database file: " << pathWithFile << "\e[0m \n";
                    *(m_options->log) << "Db::Db couldn't create database file: " << pathWithFile << "\n";
                }
                else {
                    cout << "\e[0;32m [OK]created database file: " << pathWithFile << "\e[0m \n";
                    *(m_options->log) << "[OK]created database file: " << pathWithFile << "\n";
                    openDatabase();
                    createDb();
                    closeDatabase();
                }
            }
            else {
                cout << "\e[0;33m database file: " << pathWithFile << " already exists \e[0m \n";
                openDatabase();
                closeDatabase();
            }
        }
    }

// OLD
//     if( stat( DATABASE_DIR, &st ) == 0 ) {
//         cout << "  [*]dir '" << DATABASE_DIR << "/' found\n";
//         *(m_options->log) << "  [*]dir '" << DATABASE_DIR << "/' found\n";
//     }
//     else {
//         cout << "\e[0;33m  [!]couldn't find dir '" << DATABASE_DIR << "/'! Creating dir '" << DATABASE_DIR << "/'..\e[0m \n";
//         *(m_options->log) << "  [!]couldn't find dir '" << DATABASE_DIR << "/'! Creating dir '" << DATABASE_DIR << "/'..\n";
//
//         if( mkdir( DATABASE_DIR, 0777 ) != 0 ) {
//             cout << "\e[1;31m[EPIC FAIL] couldn't create directory '" << DATABASE_DIR << "/'.Please check permissions!\e[0m \n";
//             *(m_options->log) << "[EPIC FAIL] couldn't create directory '" << DATABASE_DIR << "/'.Please check permissions!\n";
//             *(m_options->errors) << "[EPIC FAIL] On " << (*m_options)[m_options->serverNumber].getName()  << " : couldn't create directory '" << DATABASE_DIR << "/'.Please check permissions!\n";
//             cout << "\e[1;31m[FAIL] need database for bot to work correctly. Please resolve this problem and launch application again\e[0m \n";
//             *(m_options->log) << "[FAIL] can't create database! Haven't got permission to do so.. i will ignore this server.\n";
//             *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].getName()  << " : can't create database! Haven't got permission to do so.. i will ignore this server.\n";
//             //#warning TODO terminate program here if i cant create database and/or database file!!!
//         }
//         else {
//             cout << "\e[0;32m  [OK]created '" << DATABASE_DIR << "/' directory..\e[0m \n";
//             *(m_options->log) << "  [OK]created '" << DATABASE_DIR << "/' directory..\n";
//         }
//     }
/*
    //check per il database
    ifstream IN( DATABASE );

    if( !IN ) {
        cout << "\e[0;33m  [!] database doesn't exist!\e[0m \n";
        //create database
        cout << "\e[0;33m    [!] creating database '" << DATABASE << "/' in '" << DATABASE_DIR << "/'\e[0m \n";
        *(m_options->log) << "  [!] database doesn't exist!\n" << "    [*] creating database '" << DATABASE << "/' in '" << DATABASE_DIR << "/'\n";
        openDatabase();
        createDb();
    }
    else {
        IN.close();
        openDatabase();
    }

    //azzero la tabella degli admins
    loadAdminlist( admins );
    loadBanlist( banned );
    dumpAdminsToFile();
    dumpBannedToFile();*/
}


Db::~Db()
{
    #ifdef DB_DEBUG
    cout << "\e[1;37mDb::~Db()\e[0m \n";
    #endif
}


bool Db::openDatabase()
{
    if( !connect() ) {  //not opened
        cout << "\e[1;31mDb::open() FAILED to open database!\e[0m \n";
        *(m_options->log) << "Db::open() FAILED to open database!\n";
        *(m_options->errors) << "On " << (*m_options)[m_options->serverNumber].name()  << " : Db::open() FAILED to open database!\n";
        return false;
    }
    else
        return true;
}


void Db::closeDatabase()
{
    m_resultCode = sqlite3_close( m_database );

    if( m_resultCode != SQLITE_OK ) {
        cout << "\e[1;31[FAIL] Can't close database! Something's wrong\e[0m \n";
        *(m_options->log) << "[FAIL] Can't close database! Something's wrong\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Can't close database! Something's wrong\n";
    }

    #ifdef DB_DEBUG
    if( m_resultCode == SQLITE_OK )
    {
        cout << "\e[1;35mDATABASE CLOSED\e[0m \n";
    }
    else
    {
        cout << "\e[1;31mFAILED TO CLOSE DATABASE\e[0m \n";
        *(m_options->errors) << "\e[1;31mFAILED TO CLOSE DATABASE\e[0m \n";
    }
    #endif
}


bool Db::checkAuthGuid( const string &guid )    //checks oplist for ops
{

#ifdef DB_DEBUG
    cout << "Db::checkAuthGuid\n";
#endif

    string aux( "select guid from oplist where guid='" );
    aux.append( guid );
    aux.append( "'" );

    return ( resultQuery( aux ) > 0 );
}


bool Db::checkBanGuid( const string &banGuid )
{
    string query( "select guid FROM guids WHERE guid='" );
    query.append( banGuid );
    query.append( "';" );

    cout << "[-] Test sul ban " << banGuid << "\n";
    *(m_options->log) << "[-] Test sul ban "<< banGuid << "\n";

    if( resultQuery( query ) > 0 )
        return true;
    else return false;
}


bool Db::checkBanNick( const string& nick )
{

#ifdef DB_DEBUG
    cout << "Db::checkBanNick\n";
#endif

    string query( "select id from banned where nick='" );
    query.append( nick );
    query.append( "';" );

#ifdef DB_DEBUG
    cout << query << "\n";
#endif

    if( resultQuery( query ) > 0 )
        return true;
    else
        return false;
}


bool Db::isOpTableEmpty()
{
    if( resultQuery( "select * from oplist;" ) > 0 )   //got results so not empty
        return false;
    else
        return false;
}


void Db::dumpAdminsToFile()
{
    string query( "select *from oplist;" );

    cout << "\n\n[-]Trying to dump admins..\n";
    *(m_options->log) << "[-]Trying to dump admins..\n";

    if( !resultQuery( query ) ) {   //FAILED
        cout << "\e[0;33m[!]no admins to dump to file\e[0m \n";
        *(m_options->log) << "[!]no admins to dump to file\n";
        return;
    }

    //TODO trovare altro metodo invece di usare la struct "stat"
    //Prepare file to write to...( check for old file. If exists, delete it )
    //=====================
    struct stat st;

    if( stat( "backup/Adminlist.backup", &st ) == 0 ) {

        cout << "[-]deleting old adminlist dump file..\n";

        if ( !system( "rm backup/Adminlist.backup" ) )
            cout << "\e[0;33m[!]deleted old adminlist file..creating new one..\e[0m \n";
        else {
            cout << "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
            return;
        }
    }
    //=====================

    //use logger class to save info to file
    Logger *output = new Logger( "backup/Adminlist.backup" );

    if ( !output->open() ) {
        cout << "\e[0;31m[ERR] can't write Adminlist backup file!\e[0m \n";
        *(m_options->log) << "[ERR] can't write Adminlist backup file!\n";
        *(m_options->errors) << "[ERR] On " << (*m_options)[m_options->serverNumber].name()  << " : can't write Adminlist backup file!\n";
        return;
    }

    //backup file ready, preparation of info to dump to file
    vector< string > adminIds = extractData( "select id from oplist;" );
    vector< string >adminsNick, adminsGuid;

    adminsNick = extractData( "select nick from oplist;" );
    adminsGuid = extractData( "select guid from oplist;" );

    *output << "#\n# THIS IS A BACKUP FILE OF ADMINLIST FOUND IN cfg/\n#\n";

    //here i actually write to file
    for( unsigned int i = 0; i < adminsNick.size(); i++ ) { //nick.size = guid.size
        *output << "#admin" << i << "\n";
        *output << adminsNick[i] << "=" << adminsGuid[i] << "\n";
    }

    *output << "####\n#end\n";

    cout << "\e[0;32m[OK] successfully written Adminlist.backup file in 'backup/'\e[0m \n";
    *(m_options->log) << "[OK] successfully written Adminlist.backup file in 'backup/'\n";

    output->close();

    delete output;
}


void Db::dumpBannedToFile()
{
    string query( "select * from banned;" );

    cout << "\n\n[-]Trying to dump banned guid's..\n";
    *(m_options->errors) << "[-]Trying to dump banned guid's..\n";

    if( !resultQuery( query ) ) {
        cout << "\e[0;33m[!]no banned guids to dump to file\e[0m \n";
        *(m_options->log) << "[!]no banned guids to dump to file\n";
        return;
    }

    //TODO trovare altro metodo invece di usare la struct "stat"
    //Prepare file to write to...( check for old file. If exists, delete it )

    struct stat st;

    if( stat( "backup/Banlist.backup", &st ) == 0 ) {
        cout << "[-]deleting old banlist dump file..\n";
        if ( !system( "rm backup/Banlist.backup" ) )
            cout << "\e[0;33m[!]deleted old backup file..creating new one..\e[0m \n";
        else {
            cout << "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
            return;
        }
    }

    //backup file ready, preparation of info to dump to file
    vector< string > bannedIds = extractData( "select id from banned;" );
    vector< string > dataToDump;    // banned player info
    vector< string > guidsToDump;   //banned player's guids
    string auxQuery;

    if( bannedIds.empty() ) {   //no need to go ahead, no data to dump!
        cout << "\e[0;31 Db::dumpBannedToFile no banned id's to dump\e[0m \n";
        *(m_options->log) << "Db::dumpBannedToFile no banned id's to dump \n";
        return;
    }

    //use logger class to save info to file
    Logger *output = new Logger( "backup/Banlist.backup" );

    if ( !output->open() ) {
        cout << "\e[1;31mDb::dumpBannedToFile [ERR] can't write Banlist backup file!\e[0m \n";
        *(m_options->log) << "Db::dumpBannedToFile [ERR] can't write Banlist backup file!\n";
        *(m_options->errors) << "[ERR] On " << (*m_options)[m_options->serverNumber].name()  << " : can't write Banlist backup file!\n";
        return;
    }

    *output << "#\n# THIS IS A BACKUP FILE OF BANLIST FOUND IN cfg/\n#\n";
    //here i actually write to file
    for( unsigned int i = 0; i < bannedIds.size(); i++ ) {

        #ifdef DB_DEBUG   //stamps vector id's
            cout<< "id " << i << " " << bannedIds[i] <<endl;
        #endif

        dataToDump.clear();

        //get new banned player info
        auxQuery.clear();
        auxQuery.append( "select *from banned where id='" );
        auxQuery.append( bannedIds[i] );
        auxQuery.append( "';" );

        dataToDump = extractData( auxQuery );

        for( unsigned int j = 1; j < dataToDump.size(); j++ ) { // j = 1 because i start from the nick, don't need it's id
            if( j == 1 )
                *output << "nick=";
            else if( j == 2 )
                *output << "ip=";
            else if( j == 3 )
                *output << "date=";
            else if( j == 4 )
                *output << "time=";
            else if( j == 5 )
                *output << "motive=";
            else if( j == 6 )
                *output << "author=";

            if( dataToDump[j].empty() ) *output << "<empty record>\n";
            else *output << dataToDump[j] << "\n";

        #ifdef DB_DEBUG
            cout << "banned user data being dumped " << dataToDump[j] << endl;
        #endif

        }

        *output << "====\n";

        //new query to get banned players corrisponding guids
        auxQuery.clear();
        auxQuery.append( "select guid from guids where banId='" );
        auxQuery.append( bannedIds[i] );
        auxQuery.append( "';" );

        guidsToDump.clear();
        guidsToDump = extractData( auxQuery );

        for( unsigned int k = 0; k < guidsToDump.size(); k++ ) {
            *output << "GUID=" << guidsToDump[k] << "\n";

        #ifdef DB_DEBUG
            cout << "banned user guids being dumped " << guidsToDump[k] << endl;
        #endif
        }
        *output << "####\n";
    }
    *output << "#end";

    cout << "\e[0;32m[OK] successfully written Banlist.backup file in 'backup/'\e[0m \n";
    *(m_options->log) << "[OK] successfully written Banlist.backup file in 'backup/'\n";

    output->close();

    delete output;  //non mi serve più
}


void Db::dumpDatabases()
{
    //close all db connections
    closeDatabase();    // i work only on one database at a time so this should be sufficient

    //  dump all db's
    //command creation
    for( unsigned int i = 0; i < m_options->size()/*how many servers i've got*/; i++ ) {
        //create copy command
        string cmd( "cp " );

        //get current database directory
        cmd.append( (*m_options)[i].dbFolder() );
        cmd.append( DB_NAME );

        //add space
        cmd.append( " " );

        //get backup directory
        cmd.append( (*m_options)[i].backupDir() );

        if( cmd[ cmd.length() ] != '/' )
            cmd.append( "/" );

        cmd.append( DB_NAME );
        cmd.append( ".backup." );
        cmd.append( timeStamp() );  //add day,month and year to backup file name

        if( system( cmd.c_str() ) ) {  // 0 is success, if I enter here, cmd FAILED
            cout << "\e[1;31mDb::dumpDatabase database dump failed!\e[0m \n";
            *(m_options->log) << "Db::dumpDatabase database dump failed!\n";
            *(m_options->errors) << "On " << (*m_options)[m_options->serverNumber].name()  << " : Db::dumpDatabase database dump failed!\n";
            return;
        }

        //just log this call, no need for output
        *(m_options->log) << "Db::dumpDatabase SUCCESS, dumped the database\n";
    }
}


//CUSTOM QUERIES FOR ANALYZER
vector< Db::idMotiveStruct > Db::idMotiveViaGuid( const string& guid )
{
    string query( "SELECT banned.id,banned.motive,banned.date,banned.time FROM banned join guids ON banned.id=guids.banId WHERE guids.guid='" );
    query.append( guid );
    query.append( "';" );

    vector< idMotiveStruct >structs;

    vector< string >answer = extractData( query );

#ifdef DB_DEBUG
    cout << "Db::idMotiveViaGuid\n";
    for( unsigned int i = 0; i < answer.size(); i++ )
        cout << "ANSWER" << i << " is -> " << answer.at( i ) << endl;
#endif

    if( answer.size() > 0 )
        //insert in order, id, motive, date, time
        structs.push_back( idMotiveStruct( answer[0], answer[1], answer[2], answer[3] ) );

    return structs;
}

vector< Db::idMotiveStruct > Db::idMotiveViaIp( const string& ip )
{
    string query( "SELECT motive,id FROM banned WHERE ip='" );
    query.append( ip );
    query.append( "';" );

    vector< idMotiveStruct >structs;

    vector< string >answer = extractData( query );

#ifdef DB_DEBUG
    cout << "Db::idMotiveViaIp" << endl;

    for( unsigned int i = 0; i < answer.size(); i++ )
        cout << "ANSWER " << i << " is -> " << answer.at( i );
#endif
//  SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW
    if( answer.size() > 0 )
        //insert in order, id, motive, date, time
        structs.push_back( idMotiveStruct( answer[0], answer[1], answer[2], answer[3] ) );

    return structs;
}

vector< Db::idMotiveStruct > Db::idMotiveViaNick( const string& nick )
{
    string query( "select id,motive,date,time from banned where nick='" );
    query.append( nick );
    query.append( "';" );

    vector< idMotiveStruct >structs;

    vector< string >answer = extractData( query );

#ifdef DB_DEBUG
    cout << "Db::idMotiveViaNick" << endl;

    for( unsigned int i = 0; i < answer.size(); i++ )
        cout << "ANSWER " << i << " is -> " << answer.at( i );
#endif
//  SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW
    if( answer.size() > 0 )
        //insert in order, id, motive, date, time
        structs.push_back( idMotiveStruct( answer[0], answer[1], answer[2], answer[3] ) );

    return structs;
}




//BAN METHODS
bool Db::ban( const string &nick, const string &ip, const string &date, const string &time, const string &guid, const string &motive, const string &adminGuid ) //adds banned guid to database
{
    if ( checkBanGuid( guid ) ) {
        cout << "\e[0;33m[!]guid: " << guid << " already banned\e[0m \n";
        *(m_options->log) << "[!]guid: " << guid << " already banned\n";
        return false;
    }

    string adminNick = getAdminNick( adminGuid );
    string banId = insertNewBanned( nick, ip, date, time, motive, adminNick );    //get the autoincrement id

    if( banId.empty() )    // empty string is ERROR
        return false;    //didn't write to database

    if( insertNewGuid( guid, banId ).empty() )    // empty string is ERROR
        return false;

    //all went well, add admin to AUTHOR
    return true;
}


string Db::insertNewBanned( const string& nick, const string& ip, const string& date, const string &time, const string &motive, string adminNick )
{
    string newBanQuery( "insert into banned( nick, ip, date, time, motive, author ) values('" );

    if( adminNick.empty() ) {   //if no author, set it to BanBot
        adminNick.clear();
        adminNick = "BanBot";
    }

    newBanQuery.append( nick );
    newBanQuery.append( "','" );
    newBanQuery.append( ip );
    newBanQuery.append( "','" );
    newBanQuery.append( date );
    newBanQuery.append( "','" );
    newBanQuery.append( time );
    newBanQuery.append( "','" );
    newBanQuery.append( motive );
    newBanQuery.append( "','" );
    newBanQuery.append( adminNick );
    newBanQuery.append( "');" );

    if( !execQuery( newBanQuery ) )
        return string();  //FAIL return empty string

    //else return last banId
    vector< string > max;

    string selectLastInserted( "select id from banned where nick='" );
    selectLastInserted.append( nick );
    selectLastInserted.append( "' and ip='" );
    selectLastInserted.append( ip );
    selectLastInserted.append( "';" );

#ifdef DB_DEBUG
    cout << "Db::insertNewBanned : selectLastInserted query ->  " << selectLastInserted << endl;
#endif

    max = extractData( selectLastInserted );

#ifdef DB_DEBUG
    cout << "SHOWING VDATA\n";
    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << max[i] << "\n";
#endif

    if( max.empty() )
        return string();
    else return max[ 0 ];
}


bool Db::modifyBanned( const string &nick, const string &ip, const string &date, const string &time, const string &motive, const string &id )
{
    string query( "update banned set " );
    bool paramCount = false;
    //TODO semplificare questa funzione usando un vector o struct con chiave-valore

    if( !nick.empty() ) {
        query.append( "nick = '" );
        query.append( nick );
        query.append( "' " );
        paramCount = true;
    }

    if( !ip.empty() ) {
        if( paramCount )
            query.append( "," );
        query.append( "ip = '" );
        query.append( ip );
        query.append( "' " );
        paramCount = true;
    }

    if( !date.empty() ) {
        if( paramCount )
            query.append( "," );
        query.append( "date = '" );
        query.append( date );
        query.append( "' " );
        paramCount = true;
    }

    if( !time.empty() ) {
        if( paramCount )
            query.append( "," );
        query.append( "time = '" );
        query.append( time );
        query.append( "' " );
        paramCount = true;
    }

    if( !motive.empty() ) {
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
    if( !execQuery( query ) ) {
        cout << "\e[0;31m[FAIL] Db::modifyBanned : " << query << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::modifyBanned : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::modifyBanned : " << query << "\n";
        return false;
    }
    else return true;
}


bool Db::deleteBanned( const string &id )
{
    if( id.empty() ) {
        cout << "\e[0;31m Db::deleteBanned empty id! \e[0m \n";
        return false;
    }

    //first clean guid records
    string deleteGuidsQuery( "delete from guids where banId='" );
    deleteGuidsQuery.append( id );
    deleteGuidsQuery.append( "';" );

#ifdef DB_DEBUG
    cout << "Db::deleteBanned guids query -> " << deleteGuidsQuery << "\n";
#endif

    if( !execQuery( deleteGuidsQuery ) ) {
        cout << "\e[0;31m[FAIL] Db::deleteBanned can't delete guid with banId = " << id << "\e[0m \n ";
        *(m_options->log) << "[FAIL] Db::deleteBanned can't delete guid with banId = " << id << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::deleteBanned can't delete guid with banId = " << id << "\n";
        return false;
    }

    cout << "\e[0;31m[!] deleted all guids with ban id = " << id << "\e[0m \n";
    *(m_options->log) << "[!] deleted all guids with ban id = " << id << "\n";


    //and now clean banned table
    string query( "delete from banned where id ='" );
    query.append( id );
    query.append( "';" );

#ifdef DB_DEBUG
    cout << "Db::deleteBanned bantable query -> " << query << "\n";
#endif

    if( !execQuery( query ) ) {
        cout << "\e[0;31m[FAIL] Db::deleteBanned : " << query << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::deleteBanned : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::deleteBanned : " << query << "\n";
        return false;
    }

    return true;
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

    if( !guid.empty() ) {
        query.append( "guid = '" );
        query.append( guid );
        query.append( "' " );
        paramCount = true;
    }

    if( !banId.empty() ) {
        if( paramCount )
            query.append( "," );
        query.append( "banId = '" );
        query.append( banId );
        query.append( "' " );
    }

    query.append( "where id = '" );
    query.append( id );
    query.append( "';" );

    if( !execQuery( query ) ) {
        cout << "\e[0;31m[FAIL] Db::modifyGuid : " << query << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::modifyGuid : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::modifyGuid : " << query << "\n";
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
        *(m_options->log) << "[FAIL] Db::deleteGuid : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::deleteGuid : " << query << "\n";
        return false;
    }
    else return true;
}


//OPLIST TABLE METHODS
bool Db::addOp( const string& nick, const string& guid )
{
    if ( checkAuthGuid( guid ) ) {
        cout << "\e[0;33m[!]Admin: " << nick << " : " << guid << " already exists on Database\e[0m \n";
        *(m_options->log) << "[!]Admin: " << nick << " : " << guid << " already exists on Database\n";
        return false;
    }

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

    if( !nick.empty() ) {
        modifyQuery.append( "nick = '" );
        modifyQuery.append( nick );
        modifyQuery.append( "' " );
        paramCount = true;
    }

    if( !guid.empty() ) {
        if( paramCount )
            modifyQuery.append( "," );
        modifyQuery.append( "guid = '" );
        modifyQuery.append( guid );
        modifyQuery.append( "' " );
    }

    modifyQuery.append( "where id = '" );
    modifyQuery.append( id );
    modifyQuery.append( "';" );

    if( !execQuery( modifyQuery ) ) {
        cout << "\e[0;31m[FAIL] Db::modifyOp : " << modifyQuery << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::modifyOp : " << modifyQuery << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::modifyOp : " << modifyQuery << "\n";
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


vector< string > Db::extractData( const string &query )    //returns vector with results as string
{
    if( execQuery( query ) ) {
    #ifdef DB_DEBUG
        cout << "Db::extractData query -> " << query << endl;
    #endif
        return m_data;
    }
    else {
        vector< string > emptyVector;
        return emptyVector;
    }
}


/********************************
*       PRIVATE METHODS         *
********************************/
bool Db::connect()  //called by Db::open ( public function )
{
    cout << "\e[0;33m connecting to database in " << (*m_options)[m_options->serverNumber].dbFolder() << "\e[0m \n";
    //get path to current db
    string database( (*m_options)[m_options->serverNumber].dbFolder() );

    if( database[database.size()-1] != '/' )  //CAREFUL, NEED THIS!!!
        database.append( "/" );

    database.append( DB_NAME );

    if( sqlite3_open( database.c_str(), &m_database ) ) {
        cout<<"\e[0;31m[EPIC FAIL] " << sqlite3_errmsg( m_database ) << "\e[0m \n";
        *(m_options->log) <<"[FAIL] " << sqlite3_errmsg( m_database );
        *(m_options->errors) <<"[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << "@ " << database << " : " << sqlite3_errmsg( m_database );
        sqlite3_close( m_database );
        return false;
    }

#ifdef DB_DEBUG
    cout << "\e[1;35mOPENED DB\e[0m \n";
    cout << "\e[1;35musing database in " << (*m_options)[m_options->serverNumber].dbFolder() << " \e[0m \n";
#endif

    return true;
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
    "motive TEXT,"
    "author TEXT);" );

    string createGuidTable(     //for banned users
    "create table guids("
    "id INTEGER PRIMARY KEY,"    //autoincrement
    "guid TEXT,"
    "banId TEXT,"
    "FOREIGN KEY( banId ) REFERENCES banned( id ) );" );

    string createOplistTable(
    "create table oplist("
    "id INTEGER PRIMARY KEY,"   //autoincrement
    "nick TEXT,"
    "guid TEXT);" );

    //checks...
    if( resultQuery( createBannedTable ) == 0 ) {
        cout << "\e[0;32m    [*]created 'banned' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'banned' table..\n";
    }
    else {
        cout << "\e[0;31m    [FAIL]error creating 'banned' table\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'banned' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'banned' table\n";
    }

    if( resultQuery( createGuidTable ) == 0 ) {
        cout << "\e[0;32m     [*]created 'guid' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'guid' table..\n";
    }
    else {
        cout << "\e[0;31m    [FAIL]error creating 'guid' table\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'guid' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'guid' table\n";
    }

    if( resultQuery( createOplistTable ) == 0 ) {
        cout << "\e[0;32m     [*]created 'oplist' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'oplist' table..\n";
    }
    else {
        cout << "\e[0;31m    [FAIL]error creating 'oplist' table\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'oplist' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'oplist' table\n";
    }
}


string Db::errorCodeToString( int errorCode ) const
{
    string errorMsg;
    cout << "ERROR CODE RECIEVED -> " << errorCode << endl;

    if( errorCode == 1 )
        errorMsg = "SQL error or missing database";
    else if( errorCode == 2 )
        errorMsg = "Internal logic error in SQLite";
    else if( errorCode == 3 )
        errorMsg = "Access permission denied";
    else if( errorCode == 4 )
        errorMsg = "Callback routine requested an abort";
    else if( errorCode == 5 )
        errorMsg = "The database file is locked";
    else if( errorCode == 6 )
        errorMsg = "A table in the database is locked";
    else if( errorCode == 7 )
        errorMsg = "A malloc() failed";
    else if( errorCode == 8 )
        errorMsg = "Attempt to write a readonly database";
    else if( errorCode == 9 )
        errorMsg = "Operation terminated by sqlite3_interrupt()";
    else if( errorCode == 10 )
        errorMsg = "Some kind of disk I/O error occurred";
    else if( errorCode == 11 )
        errorMsg = "The database disk image is malformed";
    else if( errorCode == 12 )
        errorMsg = "NOT USED. Table or record not found";
    else if( errorCode == 13 )
        errorMsg = "Insertion failed because database is full";

    return errorMsg;

//     #define SQLITE_CANTOPEN    14   /* Unable to open the database file */
//     #define SQLITE_PROTOCOL    15   /* NOT USED. Database lock protocol error */
//     #define SQLITE_EMPTY       16   /* Database is empty */
//     #define SQLITE_SCHEMA      17   /* The database schema changed */
//     #define SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
//     #define SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
//     #define SQLITE_MISMATCH    20   /* Data type mismatch */
//     #define SQLITE_MISUSE      21   /* Library used incorrectly */
//     #define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
//     #define SQLITE_AUTH        23   /* Authorization denied */
//     #define SQLITE_FORMAT      24   /* Auxiliary database format error */
//     #define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
//     #define SQLITE_NOTADB      26   /* File opened that is not a database file */
//     #define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
//     #define SQLITE_DONE        101  /* sqlite3_step() has finished executing */*/*/

}



bool Db::execQuery( const string &query )   //executes and returns status
{

#ifdef DB_DEBUG
    cout << "Db::execQuery - " << query << "\n";
    cout << "using database @: " << (*m_options)[m_options->serverNumber].dbFolder() << endl;
#endif

    //clean old m_data...
    if( !m_data.empty() )
        m_data.clear();

    //..and old m_vcolHead just to be shure i don't get unwanted info
    if( !m_vcolHead.empty() )
        m_vcolHead.clear();

    //and also check that database connection is open!
    if( !m_database )
        connect();

    m_resultCode = sqlite3_get_table( m_database, query.c_str(), &m_result, &m_nrow, &m_ncol, &m_zErrMsg );
    bool success;

    if( m_vcolHead.size() < 0 )
        m_vcolHead.clear();

    if( m_data.size() < 0 )
        m_data.clear();

    if( m_resultCode == SQLITE_OK ) {
        for( int i = 0; i < m_ncol; ++i )
            m_vcolHead.push_back( m_result[i] );   /* First row heading */
            for( int i = 0; i < m_ncol * m_nrow; ++i )
                m_data.push_back( m_result[m_ncol+i] );

        success = true;

    #ifdef DB_DEBUG
        for( unsigned int i = 0; i < m_data.size(); i++ ) {
            cout << "VDATA INFO\n";
            cout << m_data[i] << "\n";
        }
    #endif

    }
    else {
        cout << "\e[1;31m[EPIC FAIL] Db::execQuery " << errorCodeToString( m_resultCode ) << "\e[0m \n";
//         outputSqliteError( m_resultCode );
        *(m_options->log) << "[EPIC FAIL] Db::execQuery '" << m_zErrMsg << "'\n";
        *(m_options->log) << "[ERROR MSG]" << errorCodeToString( m_resultCode ) << "\n";
        *(m_options->errors) << "[EPIC FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::execQuery '" << m_zErrMsg << "'\n";
        *(m_options->errors) << errorCodeToString( m_resultCode );
        success = false;
    }
    sqlite3_free_table( m_result );
    //or sqlite3_free_table( result );

#ifdef DB_DEBUG
    cout << "\e[1;37m Db::execQuery result code is > " << m_resultCode << " sending value " << success << "\e[0m \n";
#endif
    return success;
}

string Db::getAdminNick( const string& guid )
{
    //if empty return empty string and don't do query
    //otherwise it wil crash returning vdata[0] because it doesn't exist!
    if( guid.empty() )
        return string();

    string query( "select nick from oplist where guid='" );
    query.append( guid );
    query.append( "';" );

    if( execQuery( query ) )
        return m_data[0];
    else
        return string();
}


// void Db::loadAdminlist( vector< ConfigLoader::AdminList > admins )
// {
//     cout << "\e[0;33m[!] Loading admins to database.. \e[0m \n";
//
//     if( admins.empty() ) {
//         cout<<"\e[0;33m[!]Adminlist EMPTY\e[0m \n";
//         *(m_options->log)<<"[!]Adminlist EMPTY\n";
//         return;
//     }
//
//     int addedCounter = 0;
//
//     //adding admins to db
//     cout<<"\n[-]Adding admins to database..\n\n";
//
// #ifdef DB_DEBUG
//     cout << "ADMIN SIZE-> " << admins.size() << "\n";
// #endif
//
//     for( unsigned int i = 0; i < admins.size(); i++ ) {
//
// //        addop already checks for guid on database. Don't need to do check here!
// //        if( !checkAuthGuid( admins[i].value ) ) {    //non esiste sul database
// //            //add to database
//         if ( addOp( admins[i].name, admins[i].value ) ) {
//             cout << "\e[0;32m      [+]added admin: " << admins[i].name << "\e[0m \n";
//             *(m_options->log)<<"      [+]added admin: " << admins[i].value << "\n";
//             addedCounter++;
//         }
//     }
//     cout << "\e[0;33m Added " << addedCounter << "/" << admins.size() << " new admin/s from file to the database\e[0m \n\n";
//     *(m_options->log) << "Added " << addedCounter << " new admin/s from file to the database\n";
// }
//
//
// void Db::loadBanlist( vector<ConfigLoader::Banlist> banned )
// {
//     #ifdef DB_DEBUG
//     cout << "Db::loadBanlist\n";
//     #endif
//
//     cout << "\e[0;33m[!] Loading banlist to database.. \e[0m \n";
//
//     if( banned.empty() ) {
//         cout<<"\e[0;33m[!]Banlist EMPTY\e[0m \n";
//         *(m_options->log)<<"[!]Banlist EMPTY\n";
//         return;
//     }
//
//     int playerCounter = 0,/* onDbCounter = 0,*/ addedGuidsCounter = 0;
//
//     //adding banned users to db
//     cout<<"\n[-]Adding banned users to database..\n\n";
//
//     for( unsigned int i = 0; i < banned.size() - 1; i++ ) {  //TODO find out why it works with "-1" while for the guids i don't need the "-1"
//
//         if( !checkBanNick( banned[i].nick ) ) {   //if not on database
//
//         #ifdef DB_DEBUG
//             cout << "Db::loadAdminlist " << banned[i].nick << " not on database!\n";
//             cout << "Db::loadBanlist ban info -> " << banned[i].nick << " " << banned[i].ip << " " << banned[i].date << " " << banned[i].time << " " << banned[i].motive << " " << banned[i].author << endl;
//         #endif
//
//             string banId = insertNewBanned( banned[i].nick, banned[i].ip, banned[i].date, banned[i].time, banned[i].motive, banned[i].author );
//
//             if( banId.empty() ) {
//                 cout << "\e[0;31m[FAIL] can't add banned player: " << banned[i].nick << " to banned database!\e[0m \n";
//                 *(m_options->log) << "[FAIL] can't add banned player: " << banned[i].nick << " to banned database!\n";
//                 *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].getName()  << " : can't add banned player: " << banned[i].nick << " to banned database!\n";
//                 return;
//             }
//
//             playerCounter++; //one user added
//
//             for( unsigned int j = 0; j < banned[i].guids.size(); j++ ) {
//                 string guidId = insertNewGuid( banned[i].guids[j], banId );
//
//                 if( guidId.empty() ) {
//                     cout << "\e[0;31m[FAIL] can't add banned player's guid: " << banned[i].nick << " to database!\e[0m \n";
//                     *(m_options->log) << "[FAIL] can't add banned player's guid: " << banned[i].nick << " to database!\n";
//                     *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].getName()  << " : can't add banned player's guid: " << banned[i].nick << " to banned database!\n";
//                     return;
//                 }
//                 addedGuidsCounter++;
//             }
//         }
//     #ifdef DB_DEBUG
//         else
//             cout << banned[i].nick << " on database!\n";
//     #endif
//
//     }
//     cout << "\e[0;33m Added " << playerCounter << " clients and " << addedGuidsCounter << " new guids to the database\e[0m \n";
//     *(m_options->log) << "Added " << playerCounter << " clients and " << addedGuidsCounter << " new guids to the database \n";
// }


int Db::resultQuery( const string &query ) //ritorna quante corrispondenze ci sono all'interno del DB
{

#ifdef DB_DEBUG
    cout << "Db::resultQuery\n";
    cout << "query to check : " << query << "\n";
#endif

    int answer = 0;

    if( !execQuery( query ) ) {
        cout << "query failed\n";
        answer = -1;
    }
    else
        answer = m_data.size();

#ifdef DB_DEBUG
    cout << "\e[1;37mDb::resultQuery ANSWER SIZE > " << m_data.size() << "\e[0m \n";
    cout << "\e[1;37mDb::resultQuery ANSWER SENDING OUT IS > " << answer << "\e[0m \n";
#endif

    return answer;
}