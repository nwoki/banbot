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

    checkDatabases();   /* this does it all */
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
        cout << "\e[1;31m[FAIL] Can't close database! Something's wrong\e[0m \n";
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


int Db::checkAuthGuid( const string &guid )    //checks oplist for ops
{

#ifdef DB_DEBUG
    cout << "Db::checkAuthGuid\n";
#endif

    string query( "select level from oplist where guid='" );
    query.append( guid );
    query.append( "'" );

    int convertedInt = 100;     // default value

    if( execQuery( query ) ) {
        if( !m_data.empty() )
            convertedInt = stringToInt( m_data[0] );
    }

    return convertedInt;        // 100 if not found
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


void Db::checkDatabases()
{
    /* check all server objects to see if the dir of their database is valid and creates databases in case
     * useful for when adding a new server to check "on the fly
     */

    for( unsigned int i = 0; i < m_options->servers.size(); i++ ) {
        m_options->serverNumber = i;    /* set server number I'm working onDbCounter */

        // get path to db folder
        string dbPath = m_options->servers.at( i )->dbFolder();

        if( dbPath[dbPath.size()-1] != '/' )
            dbPath.append( "/" );

        string pathWithFile( dbPath ); /* set full file path */
        pathWithFile.append( DB_NAME );

        /* DIRECTORY CHECK */
        if( !fileExistance( dbPath ) ) {    // does directory exist?
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

        /* FILE CHECK */
        if( !fileExistance( pathWithFile ) ) {  // does file exist?
            if( !fileCreate( pathWithFile ) ) { // create database if needed
                cout << "\e[1;31mDb::Db couldn't create database file: " << pathWithFile << "\e[0m \n";
                *(m_options->log) << "Db::Db couldn't create database file: " << pathWithFile << "\n";
                (*m_options)[i].setValid( false );   // set database validity
            }
            else {
                cout << "\e[0;32m [OK]created database file: " << pathWithFile << "\e[0m \n";
                *(m_options->log) << "[OK]created database file: " << pathWithFile << "\n";
                openDatabase();
                createDb();
                closeDatabase();
                (*m_options)[i].setValid( true );   // set database validity
            }
        }
        else {
            cout << "\e[0;33m database file: " << pathWithFile << " already exists \e[0m \n";
            openDatabase();
            closeDatabase();
            (*m_options)[i].setValid( true );   // set database validity
        }
    }

    m_options->serverNumber = 0;    // set server number back to start ( 0 )!!!
}


bool Db::isOpTableEmpty()
{
    if( resultQuery( "select * from oplist;" ) > 0 )   //got results so not empty
        return false;
    else
        return true;
}


// void Db::dumpAdminsToFile()
// {
//     string query( "select *from oplist;" );
//
//     cout << "\n\n[-]Trying to dump admins..\n";
//     *(m_options->log) << "[-]Trying to dump admins..\n";
//
//     if( !resultQuery( query ) ) {   //FAILED
//         cout << "\e[0;33m[!]no admins to dump to file\e[0m \n";
//         *(m_options->log) << "[!]no admins to dump to file\n";
//         return;
//     }
//
//     //TODO trovare altro metodo invece di usare la struct "stat"
//     //Prepare file to write to...( check for old file. If exists, delete it )
//     //=====================
//     struct stat st;
//
//     if( stat( "backup/Adminlist.backup", &st ) == 0 ) {
//
//         cout << "[-]deleting old adminlist dump file..\n";
//
//         if ( !system( "rm backup/Adminlist.backup" ) )
//             cout << "\e[0;33m[!]deleted old adminlist file..creating new one..\e[0m \n";
//         else {
//             cout << "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
//             return;
//         }
//     }
//     //=====================
//
//     //use logger class to save info to file
//     Logger *output = new Logger( "backup/Adminlist.backup" );
//
//     if ( !output->open() ) {
//         cout << "\e[0;31m[ERR] can't write Adminlist backup file!\e[0m \n";
//         *(m_options->log) << "[ERR] can't write Adminlist backup file!\n";
//         *(m_options->errors) << "[ERR] On " << (*m_options)[m_options->serverNumber].name()  << " : can't write Adminlist backup file!\n";
//         return;
//     }
//
//     //backup file ready, preparation of info to dump to file
//     vector< string > adminIds = extractData( "select id from oplist;" );
//     vector< string >adminsNick, adminsGuid;
//
//     adminsNick = extractData( "select nick from oplist;" );
//     adminsGuid = extractData( "select guid from oplist;" );
//
//     *output << "#\n# THIS IS A BACKUP FILE OF ADMINLIST FOUND IN cfg/\n#\n";
//
//     //here i actually write to file
//     for( unsigned int i = 0; i < adminsNick.size(); i++ ) { //nick.size = guid.size
//         *output << "#admin" << i << "\n";
//         *output << adminsNick[i] << "=" << adminsGuid[i] << "\n";
//     }
//
//     *output << "####\n#end\n";
//
//     cout << "\e[0;32m[OK] successfully written Adminlist.backup file in 'backup/'\e[0m \n";
//     *(m_options->log) << "[OK] successfully written Adminlist.backup file in 'backup/'\n";
//
//     output->close();
//
//     delete output;
// }
//
//
// void Db::dumpBannedToFile()
// {
//     string query( "select * from banned;" );
//
//     cout << "\n\n[-]Trying to dump banned guid's..\n";
//     *(m_options->errors) << "[-]Trying to dump banned guid's..\n";
//
//     if( !resultQuery( query ) ) {
//         cout << "\e[0;33m[!]no banned guids to dump to file\e[0m \n";
//         *(m_options->log) << "[!]no banned guids to dump to file\n";
//         return;
//     }
//
//     //TODO trovare altro metodo invece di usare la struct "stat"
//     //Prepare file to write to...( check for old file. If exists, delete it )
//
//     struct stat st;
//
//     if( stat( "backup/Banlist.backup", &st ) == 0 ) {
//         cout << "[-]deleting old banlist dump file..\n";
//         if ( !system( "rm backup/Banlist.backup" ) )
//             cout << "\e[0;33m[!]deleted old backup file..creating new one..\e[0m \n";
//         else {
//             cout << "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
//             return;
//         }
//     }
//
//     //backup file ready, preparation of info to dump to file
//     vector< string > bannedIds = extractData( "select id from banned;" );
//     vector< string > dataToDump;    // banned player info
//     vector< string > guidsToDump;   //banned player's guids
//     string auxQuery;
//
//     if( bannedIds.empty() ) {   //no need to go ahead, no data to dump!
//         cout << "\e[0;31 Db::dumpBannedToFile no banned id's to dump\e[0m \n";
//         *(m_options->log) << "Db::dumpBannedToFile no banned id's to dump \n";
//         return;
//     }
//
//     //use logger class to save info to file
//     Logger *output = new Logger( "backup/Banlist.backup" );
//
//     if ( !output->open() ) {
//         cout << "\e[1;31mDb::dumpBannedToFile [ERR] can't write Banlist backup file!\e[0m \n";
//         *(m_options->log) << "Db::dumpBannedToFile [ERR] can't write Banlist backup file!\n";
//         *(m_options->errors) << "[ERR] On " << (*m_options)[m_options->serverNumber].name()  << " : can't write Banlist backup file!\n";
//         return;
//     }
//
//     *output << "#\n# THIS IS A BACKUP FILE OF BANLIST FOUND IN cfg/\n#\n";
//     //here i actually write to file
//     for( unsigned int i = 0; i < bannedIds.size(); i++ ) {
//
//         #ifdef DB_DEBUG   //stamps vector id's
//             cout<< "id " << i << " " << bannedIds[i] <<endl;
//         #endif
//
//         dataToDump.clear();
//
//         //get new banned player info
//         auxQuery.clear();
//         auxQuery.append( "select *from banned where id='" );
//         auxQuery.append( bannedIds[i] );
//         auxQuery.append( "';" );
//
//         dataToDump = extractData( auxQuery );
//
//         for( unsigned int j = 1; j < dataToDump.size(); j++ ) { // j = 1 because i start from the nick, don't need it's id
//             if( j == 1 )
//                 *output << "nick=";
//             else if( j == 2 )
//                 *output << "ip=";
//             else if( j == 3 )
//                 *output << "date=";
//             else if( j == 4 )
//                 *output << "time=";
//             else if( j == 5 )
//                 *output << "motive=";
//             else if( j == 6 )
//                 *output << "author=";
//
//             if( dataToDump[j].empty() ) *output << "<empty record>\n";
//             else *output << dataToDump[j] << "\n";
//
//         #ifdef DB_DEBUG
//             cout << "banned user data being dumped " << dataToDump[j] << endl;
//         #endif
//
//         }
//
//         *output << "====\n";
//
//         //new query to get banned players corrisponding guids
//         auxQuery.clear();
//         auxQuery.append( "select guid from guids where banId='" );
//         auxQuery.append( bannedIds[i] );
//         auxQuery.append( "';" );
//
//         guidsToDump.clear();
//         guidsToDump = extractData( auxQuery );
//
//         for( unsigned int k = 0; k < guidsToDump.size(); k++ ) {
//             *output << "GUID=" << guidsToDump[k] << "\n";
//
//         #ifdef DB_DEBUG
//             cout << "banned user guids being dumped " << guidsToDump[k] << endl;
//         #endif
//         }
//         *output << "####\n";
//     }
//     *output << "#end";
//
//     cout << "\e[0;32m[OK] successfully written Banlist.backup file in 'backup/'\e[0m \n";
//     *(m_options->log) << "[OK] successfully written Banlist.backup file in 'backup/'\n";
//
//     output->close();
//
//     delete output;  //non mi serve più
// }


void Db::dumpDatabases()
{
    //close all db connections
    //closeDatabase();    // i work only on one database at a time so this should be sufficient

    //  dump all db's
    //command creation
    for( unsigned int i = 0; i < m_options->size()/*how many servers i've got*/; i++ ) {
        //create copy command

        //get current database directory
        string source( (*m_options)[i].dbFolder() );
        if( source[ source.length()-1 ] != '/' )
            source.append( "/" );
        source.append( DB_NAME );

        /*//get backup directory
        cmd.append( (*m_options)[i].backupDir() );

        if( cmd[ cmd.length()-1 ] != '/' )
            cmd.append( "/" );*/

        string dest( (*m_options)[i].dbFolder() );

        if( dest[ dest.length()-1 ] != '/' )
            dest.append( "/" );

        dest.append( DB_NAME );
        dest.append( ".backup-" );
        dest.append( (*m_options)[i].name() );
        //cmd.append( timeStamp() );  //add day,month and year to backup file name

        //if( system( cmd.c_str() ) ) {  // 0 is success, if I enter here, cmd FAILED
        if( !copyFile( source, dest ) ) {
            cout << "\e[1;31mDb::dumpDatabase " << source << " to " << dest << " database dump failed!\e[0m \n";
            *(m_options->errors) << "On " << (*m_options)[m_options->serverNumber].name()  << " : Db::dumpDatabase " << source << " to " << dest << " database dump failed!\n";
            return;
        }
        else {
            //just log this call, no need for output
            *(m_options->errors) << "Db::dumpDatabase SUCCESS, dumped the database\n";

            #ifdef DB_DEBUG
                *(m_options->errors) << "Checking the file...";
                if( fileExistance( dest ) )
                    *(m_options->errors) << "It exists!\n";
                else
                    *(m_options->errors) << "Fail! Not found!\n";
            #endif
        }
    }
}


/********************************
*         BAN  METHODS          *
********************************/
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
    string selectLastInserted( "select id from banned where nick='" );
    selectLastInserted.append( nick );
    selectLastInserted.append( "' and ip='" );
    selectLastInserted.append( ip );
    selectLastInserted.append( "';" );

#ifdef DB_DEBUG
    cout << "Db::insertNewBanned : selectLastInserted query ->  " << selectLastInserted << endl;
#endif

    /* get max */
    execQuery( selectLastInserted );

#ifdef DB_DEBUG
    cout << "SHOWING VDATA\n";
    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << m_data[i] << "\n";
#endif

    if( m_data.empty() )
        return string();
    else
        return m_data[0];
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


/********************************
*      GUID TABLE METHODS       *
********************************/
string Db::insertNewGuid( const string& guid, const string &banId )
{
    string newGuidQuery( "insert into guids( guid, banId ) values('" );

    newGuidQuery.append( guid );
    newGuidQuery.append( "','" );
    newGuidQuery.append( banId );
    newGuidQuery.append( "');" );

    if( !execQuery( newGuidQuery ) )
        return string();  /* FAIL return empty string */

    // else return last banId
    execQuery( "select max( id ) from guids" );
    return m_data[0];
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


/********************************
*     OPLIST TABLE METHODS      *
********************************/
bool Db::addOp( const string& nick, const string& guid, const string &opLvl )
{
    if ( checkAuthGuid( guid ) > 100 ) {
        cout << "\e[0;33m[!]Admin: " << nick << " : " << guid << " already exists on Database\e[0m \n";
        *(m_options->log) << "[!]Admin: " << nick << " : " << guid << " already exists on Database\n";
        return false;
    }

    string addOpQuery( "insert into oplist( nick, guid, level ) values('" );
    addOpQuery.append( nick );
    addOpQuery.append( "','" );
    addOpQuery.append( guid );
    addOpQuery.append( "','" );
    addOpQuery.append( opLvl );
    addOpQuery.append( "');" );

    return execQuery( addOpQuery ); //true on success
}


bool Db::modifyOp( const string& id, const string& nick, const string& guid, const string &opLvl )
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
        paramCount = true;
    }

    if( !opLvl.empty() ) {
        if( paramCount )
            modifyQuery.append( "," );
        modifyQuery.append( "level='" );
        modifyQuery.append( opLvl );
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


/********************************
* CUSTOM QUERIES FOR ANALYZER   *
********************************/

/* id motive queries */
vector< Db::idMotiveStruct > Db::idMotiveViaGuid( const string& guid )
{
    string query( "SELECT banned.id,banned.motive,banned.date,banned.time FROM banned join guids ON banned.id=guids.banId WHERE guids.guid='" );
    query.append( guid );
    query.append( "' order by banned.date DESC, banned.time DESC;" );

    vector< idMotiveStruct >structs;

    if( !execQuery( query ) )   /* on fail, return immediatly empty struct */
        return structs;

    #ifdef DB_DEBUG
    cout << "Db::idMotiveViaGuid\n";
    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << "ANSWER" << i << " is -> " << m_data.at( i ) << endl;
    #endif

    if( m_data.size() > 0 )
        structs.push_back( idMotiveStruct( m_data[0], m_data[1], m_data[2], m_data[3] ) );  /* insert in order, id, motive, date, time */

    return structs;
}

vector< Db::idMotiveStruct > Db::idMotiveViaIp( const string& ip )
{
    string query( "SELECT id,motive,date,time FROM banned WHERE ip='" );
    query.append( ip );
    query.append( "' order by date DESC, time DESC;" );

    vector< idMotiveStruct >structs;

    if( !execQuery( query ) )
        return structs; /* on fail, return immediatly empty struct */

    #ifdef DB_DEBUG
    cout << "Db::idMotiveViaIp" << endl;

    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << "ANSWER " << i << " is -> " << m_data.at( i );
    #endif
    //  SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW
    if( m_data.size() > 0 )
        structs.push_back( idMotiveStruct( m_data[0], m_data[1], m_data[2], m_data[3] ) );  /* insert in order, id, motive, date, time */

    return structs;
}

vector< Db::idMotiveStruct > Db::idMotiveViaNick( const string& nick )
{

    string query( "select id,motive,date,time from banned where nick='" );
    query.append( nick );
    query.append( "' order by date DESC, time DESC;" );

    vector< idMotiveStruct >structs;

    if( !execQuery( query ) )
        return structs; /* on fail, return immediatly empty struct */


    #ifdef DB_DEBUG
    cout << "Db::idMotiveViaNick" << endl;

    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << "ANSWER " << i << " is -> " << m_data.at( i );
    #endif
    //  SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW
    if( m_data.size() > 0 )
        structs.push_back( idMotiveStruct( m_data[0], m_data[1], m_data[2], m_data[3] ) );  /* insert in order, id, motive, date, time */

    return structs;
}

/* "how many" queries */
string Db::autoBanned()
{
    string query( "select count(*) from banned where author='BanBot' group by id;" );

    #ifdef DB_DEBUG
    cout << "Db::autoBanned returning value " << resultQuery( query ) << endl;
    #endif

    return intToString( resultQuery( query ) );
}

string Db::banned()
{
    string query( "select count(*) from banned group by id;" );

    #ifdef DB_DEBUG
    cout << "Db::banned returning value " << resultQuery( query ) << endl;
    #endif

    return intToString( resultQuery( query ) );
}

string Db::ops()
{
    string query( "select count(*) from oplist group by id;" );

    #ifdef DB_DEBUG
    cout << "Db::ops returning value " << resultQuery( query ) << endl;
    #endif

    return intToString( resultQuery( query ) );
}

string Db::adminRegisteredNickViaGuid( const string& guid )
{
    string query( "select nick from oplist where guid='" );
    query.append( guid );
    query.append( "';" );

    if( execQuery( query ) ) {
        if( !m_data[0].empty() )
            return m_data[0];
    }

    // in both failed cases i return an empty string
    return string();
}


/* find queries */
vector< Db::idNickMotiveAuthorStruct > Db::findAproxIdMotiveAuthorViaNickBanned( const string& nick )
{
    string query( "select id,nick,motive,author from banned where nick like '%" );
    query.append( nick );
    query.append( "%' limit 16;" );

    vector< idNickMotiveAuthorStruct > structs;

    if( !execQuery( query ) )
        return structs;  /* on fail, return immediatly empty struct */

    #ifdef DB_DEBUG
    cout << "Db::findAproxIdMotiveAuthorViaNickBanned" << endl;
    cout << "mdata SIZE IS -> " << m_data.size() << endl;

    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << "ANSWER " << i << " is -> " << m_data.at( i );
    #endif

    for( unsigned int i = 0; i < m_data.size()/4; i++ ) {
        cout << "TEST for segfault\n";
        structs.push_back( idNickMotiveAuthorStruct( m_data.at(4*i + 0), m_data.at(4*i + 1), m_data.at(4*i + 2), m_data.at(4*i + 3) ) );
    }

    return structs;
}

vector< Db::idNickMotiveAuthorStruct > Db::findPreciseIdMotiveAuthorViaNickBanned( const string &nick )
{
    string query( "select id,nick,motive,author from banned where nick='" );
    query.append( nick );
    query.append( "' limit 7;" );

    vector< idNickMotiveAuthorStruct > structs;

    if( !execQuery( query ) )
        return structs; /* on fail, return immediatly empty struct */

    #ifdef DB_DEBUG
    cout << "Db::findPreciseIdMotiveAuthorViaNick" << endl;

    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << "ANSWER " << i << " is -> " << m_data.at( i );
    #endif

    cout << "ANSWER SIZE IS -> " << m_data.size() << endl;

    /* SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW */
    if( m_data.size() > 0 )
        structs.push_back( idNickMotiveAuthorStruct( m_data[0], m_data[1], m_data[2], m_data[3] ) );    /* insert in order, id, nick, motive, author */

    return structs;
}

vector< Db::idNickStruct > Db::findAproxIdNickViaNickOp( const string& nick )
{
    string query( "select id,nick,level from oplist where nick like '%" );
    query.append( nick );
    query.append( "%' limit 16;" );

    vector< idNickStruct > structs;

    if( !execQuery( query ) )
        return structs;     /* on fail, return immediatly empty struct */

    #ifdef DB_DEBUG
    cout << "Db::findAproxIdNickViaNickOp" << endl;

    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << "ANSWER " << i << " is -> " << m_data.at( i );
    #endif

    for( unsigned int i = 0; i < m_data.size()/3; i++ )
        structs.push_back( idNickStruct( m_data[3*i + 0], m_data[3*i + 1], m_data[3*i + 2] ) );

    return structs;
}

vector< Db::idNickStruct > Db::findPreciseIdNickViaNickOp( const string& nick )
{
    string query( "select id,nick from oplist where nick='" );
    query.append( nick );
    query.append( "' limit 7;" );

    vector< idNickStruct > structs;

    if( !execQuery( query ) )
        return structs;     /* on fail, return immediatly empty struct */

    #ifdef DB_DEBUG
    cout << "Db::findPreciseIdNickViaNickOp" << endl;

    for( unsigned int i = 0; i < m_data.size(); i++ )
        cout << "ANSWER " << i << " is -> " << m_data.at( i );
    #endif

    /* SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW */
    if( m_data.size() > 0 )
        structs.push_back( idNickStruct( m_data[0], m_data[1], m_data[2] ) );

    return structs;
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

bool Db::copyFile( const string &source, const string &destination )
{
    ifstream src;   // the source file
    ofstream dest;  // the destination file

    src.open( source.c_str(), std::ios::binary );   // open in binary to prevent jargon at the end of the buffer
    dest.open( destination.c_str(), std::ios::binary );  // same again, binary

    if( !src.is_open() || !dest.is_open() )
        return false;   // could not be copied

    dest << src.rdbuf ();   // copy the content
    dest.close ();  // close destination file
    src.close ();   // close source file

    return true; // file copied successfully
}


void Db::createDb()   //initial creation of database
{
    string createBannedTable(
    "create table banned("
    "id INTEGER PRIMARY KEY,"    // autoincrement
    "nick TEXT,"
    "ip TEXT,"
    "date TEXT,"
    "time TEXT,"
    "motive TEXT,"
    "author TEXT);" );

    string createGuidTable(     // for banned users
    "create table guids("
    "id INTEGER PRIMARY KEY,"   // autoincrement
    "guid TEXT,"
    "banId TEXT,"
    "FOREIGN KEY( banId ) REFERENCES banned( id ) );" );

    string createOplistTable(
    "create table oplist("
    "id INTEGER PRIMARY KEY,"   //autoincrement
    "nick TEXT,"
    "guid TEXT,"
    "level TEXT );" );

    //checks...
    if( resultQuery( createBannedTable ) == 0 ) {
        cout << "\e[0;32m    [*]created 'banned' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'banned' table..\n";
    }
    else {
        cout << "\e[0;31m    [FAIL]error creating 'banned' table on database @ : " << (*m_options)[m_options->serverNumber].name() << "\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'banned' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'banned' table\n";
    }

    if( resultQuery( createGuidTable ) == 0 ) {
        cout << "\e[0;32m     [*]created 'guid' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'guid' table..\n";
    }
    else {
        cout << "\e[0;31m    [FAIL]error creating 'guid' table on database @ : " << (*m_options)[m_options->serverNumber].name() << "\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'guid' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'guid' table\n";
    }

    if( resultQuery( createOplistTable ) == 0 ) {
        cout << "\e[0;32m     [*]created 'oplist' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'oplist' table..\n";
    }
    else {
        cout << "\e[0;31m    [FAIL]error creating 'oplist' table on database @ : " << (*m_options)[m_options->serverNumber].name() << "\e[0m \n";
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
    else if( errorCode == 14 )
        errorMsg = "Unable to open the database file";
    else if( errorCode == 15 )
        errorMsg = "NOT USED. Database lock protocol error";
    else if( errorCode == 16 )
        errorMsg = "Database is empty";
    else if( errorCode == 17 )
        errorMsg = "The database schema changed";
    else if( errorCode == 18 )
        errorMsg = "String or BLOB exceeds size limit";
    else if( errorCode == 19 )
        errorMsg = "Abort due to constraint violation";
    else if( errorCode == 20 )
        errorMsg = "Data type mismatch";
    else if( errorCode == 21 )
        errorMsg = "Library used incorrectly";
    else if( errorCode == 22 )
        errorMsg = "Uses OS features not supported on host";
    else if( errorCode == 23 )
        errorMsg = "Authorization denied";
    else if( errorCode == 24 )
        errorMsg = "Auxiliary database format error";
    else if( errorCode == 25 )
        errorMsg = "2nd parameter to sqlite3_bind out of range";
    else if( errorCode == 26 )
        errorMsg = "File opened that is not a database file";
    else if( errorCode == 100 )
        errorMsg = "sqlite3_step() has another row ready";
    else if( errorCode == 101 )
        errorMsg = "sqlite3_step() has finished executing";


    return errorMsg;
}



bool Db::execQuery( const string &query )  //executes and returns status
{

#ifdef DB_DEBUG
    cout << "Db::execQuery - " << query << "\n";
    cout << "using database @: " << (*m_options)[m_options->serverNumber].dbFolder() << endl;
#endif

    //clean old m_data... MUST clear otherwise i keep old values
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

    if( m_resultCode == SQLITE_OK ) {   // went well and got data
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
        cout << "\e[1;31m[EPIC FAIL] Db::execQuery " << sqlite3_errmsg( m_database ) << "\e[0m \n";
        *(m_options->log) << "[EPIC FAIL] Db::execQuery '" << m_zErrMsg << "'\n";
        *(m_options->log) << "[ERROR MSG]" << sqlite3_errmsg( m_database ) << "\n";
        *(m_options->errors) << "[EPIC FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::execQuery '" << m_zErrMsg << "'\n";
        *(m_options->errors) << sqlite3_errmsg( m_database );
        success = false;
    }
    sqlite3_free_table( m_result );

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
