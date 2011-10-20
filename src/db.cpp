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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/

#include <stdlib.h>
#include "ConfigLoader.h"
#include "db.h"
#include "logger.h"


// database versions
// ver_1.1
#define BANNED_1_1 "CREATE TABLE banned(id INTEGER PRIMARY KEY,nick TEXT,ip TEXT,date TEXT,time TEXT,motive TEXT,author TEXT)"
#define GUIDS_1_1 "CREATE TABLE guids(id INTEGER PRIMARY KEY,guid TEXT,banId TEXT,FOREIGN KEY( banId ) REFERENCES banned( id ) )"
#define OPLIST_1_1 "CREATE TABLE oplist(id INTEGER PRIMARY KEY,nick TEXT,guid TEXT)"

// ver_1.2
#define BANNED_1_2 "CREATE TABLE banned(id INTEGER PRIMARY KEY,nick TEXT,ip TEXT,date TEXT,time TEXT,motive TEXT,author TEXT)"
#define GUIDS_1_2 "CREATE TABLE guids(id INTEGER PRIMARY KEY,guid TEXT,banId TEXT,FOREIGN KEY( banId ) REFERENCES banned( id ) )"
#define OPLIST_1_2 "CREATE TABLE oplist(id INTEGER PRIMARY KEY,nick TEXT,guid TEXT, level TEXT DEFAULT '0')"

/********************************
*       PUBLIC  METHODS         *
********************************/

Db::Db(ConfigLoader::Options* conf)
    : m_database(0)
    , m_options(conf)
    , m_zErrMsg(0)
    , m_result(0)
    , m_resultOccurrences(0)
{
    // set db latest version
    m_latestVersion = VER_1_2;

    std::cout << "[*]Starting up...\n";
    std::cout << "[-]checking for database directory..\n";
    *(m_options->log) << "\n**************************************************************************************\n";
    m_options->log->timestamp();
    *(m_options->log) << "\n[*]Starting up...\n";
    *(m_options->log) << "[-]checking for database directory..\n";

    #ifdef DB_DEBUG
    //config output
    std::cout << "SETTINGS ***** \n";
    std::cout << m_options->toString();

    #endif

    checkDatabases();   /* this does it all */
}


Db::~Db()
{
    #ifdef DB_DEBUG
    std::cout << "\e[1;37mDb::~Db()\e[0m \n";
    #endif
}


bool Db::openDatabase()
{
    if (!connect()) {  //not opened
        std::cout << "\e[1;31mDb::open() FAILED to open database!\e[0m \n";
        *(m_options->log) << "Db::open() FAILED to open database!\n";
        *(m_options->errors) << "On " << (*m_options)[m_options->serverNumber].name()  << " : Db::open() FAILED to open database!\n";
        return false;
    } else {
        return true;
    }
}


void Db::closeDatabase()
{
    m_resultCode = sqlite3_close(m_database);

    if (m_resultCode != SQLITE_OK) {
        std::cout << "\e[1;31m[FAIL] Can't close database! Something's wrong\e[0m \n";
        *(m_options->log) << "[FAIL] Can't close database! Something's wrong\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Can't close database! Something's wrong\n";
    }

    #ifdef DB_DEBUG
    if (m_resultCode == SQLITE_OK) {
        std::cout << "\e[1;35mDATABASE CLOSED\e[0m \n";
    } else {
        std::cout << "\e[1;31mFAILED TO CLOSE DATABASE\e[0m \n";
        *(m_options->errors) << "\e[1;31mFAILED TO CLOSE DATABASE\e[0m \n";
    }
    #endif
}


int Db::checkAuthGuid(const std::string &guid)    //checks oplist for ops
{

#ifdef DB_DEBUG
    std::cout << "Db::checkAuthGuid\n";
#endif

    std::string query("select level from oplist where guid='");
    query.append(guid);
    query.append("'");

    int convertedInt = 100;     // default value

    if (execQuery(query) && !m_data.empty()) {
        convertedInt = atoi(m_data[0].c_str());
        #ifdef DB_DEBUG
        std::cout << "Level from db: "<<m_data[0]<<" converted in "<<convertedInt<<"\n";
        #endif
    }

    return convertedInt;        // 100 if not found
}


bool Db::checkBanGuid(const std::string &banGuid)
{
    std::string query("select guid FROM guids WHERE guid='");
    query.append(banGuid);
    query.append("';");

    std::cout << "[-] Test sul ban " << banGuid << "\n";
    *(m_options->log) << "[-] Test sul ban "<< banGuid << "\n";

    if (resultQuery(query) > 0) {
        return true;
    } else {
        return false;
    }
}


bool Db::checkBanNick(const std::string& nick)
{

#ifdef DB_DEBUG
    std::cout << "Db::checkBanNick\n";
#endif

    // don't interrogate db if nick is empty
    if (nick.empty()) {
        return false;
    }

    std::string query("select id from banned where nick='");
    query.append(nick);
    query.append("';");

#ifdef DB_DEBUG
    std::cout << query << "\n";
#endif

    if (resultQuery(query) > 0) {
        return true;
    } else {
        return false;
    }
}


void Db::checkDatabases()
{
    /*
     * check all server objects to see if the dir of their database is valid and creates databases in case
     * useful for when adding a new server to check "on the fly
     */

    for (unsigned int i = 0; i < m_options->size(); ++i) {
        m_options->serverNumber = i;    // set server number I'm working onDbCounter

        // get path to db folder
        std::string dbPath = (*m_options)[i].dbFolder();

        if (dbPath[dbPath.size()-1] != '/') {
            dbPath.append("/");
        }

        std::string pathWithFile(dbPath); /* set full file path */
        pathWithFile.append(DB_NAME);

        /* DIRECTORY CHECK */
        if (!handyFunctions::fileOrDirExistance(dbPath)) {    // does directory exist?
            if (!handyFunctions::createDir(dbPath)) {
                std::cout << "\e[1;31mDb::Db couldn't create database folder: " << dbPath << "\e[0m \n";
                *(m_options->errors) << "Db::Db couldn't create database folder: " << dbPath << "\n";
            } else {
                std::cout << "\e[0;32m  [OK]created " << dbPath << " directory..\e[0m \n";
                *(m_options->errors) << "  [OK]created " << dbPath << " directory..\n";
            }
        } else {
            std::cout << "\e[0;33m database folder: " << dbPath << " already exists \e[0m \n";
        }

        /* FILE CHECK */
        if (!handyFunctions::fileOrDirExistance(pathWithFile)) {  // does file exist?
            if (!handyFunctions::createFile(pathWithFile)) { // create database if needed
                std::cout << "\e[1;31mDb::Db couldn't create database file: " << pathWithFile << "\e[0m \n";
                *(m_options->errors) << "Db::Db couldn't create database file: " << pathWithFile << "\n";
                (*m_options)[i].setValid(false);   // set database validity
            } else {
                std::cout << "\e[0;32m [OK]created database file: " << pathWithFile << "\e[0m \n";
                *(m_options->errors) << "[OK]created database file: " << pathWithFile << "\n";
                openDatabase();
                createDb();
                closeDatabase();
                (*m_options)[i].setValid(true);   // set database validity
            }
        } else {
            std::cout << "\e[0;33m database file: " << pathWithFile << " already exists \e[0m \n";
            openDatabase();
            if (checkDbVersion() == m_latestVersion) {       // last valid database version used by BanBot
                // set database validity
                (*m_options)[i].setValid(true);
            } else {
                #ifdef ITA
                std::cout<< "\e[1;31m Versione del database errata. Leggere il manuale per vedere come convertirlo.\e[0m \n";
                *(m_options->errors) << "Versione del database errata. Leggere il manuale per vedere come convertirlo.\n";
                #else
                std::cout<< "\e[1;31m Wrong Db version: read the manual to see how to convert it.\e[0m \n";
                *(m_options->errors) << "Wrong Db version: read the manual to see how to convert it.\n";
                #endif
                (*m_options)[i].setValid(false);  // server is invalid due to database inconsistency
            }
            closeDatabase();
        }
    }

    m_options->serverNumber = 0;    // set server number back to start ( 0 )!!!
}


bool Db::isOpTableEmpty()
{
    if (resultQuery("select * from oplist;") > 0) {   //got results so not empty
        return false;
    } else {
        return true;
    }
}


// void Db::dumpAdminsToFile()
// {
//     std::string query( "select *from oplist;" );
//
//     std::cout << "\n\n[-]Trying to dump admins..\n";
//     *(m_options->log) << "[-]Trying to dump admins..\n";
//
//     if( !resultQuery( query ) ) {   //FAILED
//         std::cout << "\e[0;33m[!]no admins to dump to file\e[0m \n";
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
//         std::cout << "[-]deleting old adminlist dump file..\n";
//
//         if ( !system( "rm backup/Adminlist.backup" ) )
//             std::cout << "\e[0;33m[!]deleted old adminlist file..creating new one..\e[0m \n";
//         else {
//             std::cout << "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
//             return;
//         }
//     }
//     //=====================
//
//     //use logger class to save info to file
//     Logger *output = new Logger( "backup/Adminlist.backup" );
//
//     if ( !output->open() ) {
//         std::cout << "\e[0;31m[ERR] can't write Adminlist backup file!\e[0m \n";
//         *(m_options->log) << "[ERR] can't write Adminlist backup file!\n";
//         *(m_options->errors) << "[ERR] On " << (*m_options)[m_options->serverNumber].name()  << " : can't write Adminlist backup file!\n";
//         return;
//     }
//
//     //backup file ready, preparation of info to dump to file
//     std::vector< std::string > adminIds = extractData( "select id from oplist;" );
//     std::vector< std::string >adminsNick, adminsGuid;
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
//     std::cout << "\e[0;32m[OK] successfully written Adminlist.backup file in 'backup/'\e[0m \n";
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
//     std::string query( "select * from banned;" );
//
//     std::cout << "\n\n[-]Trying to dump banned guid's..\n";
//     *(m_options->errors) << "[-]Trying to dump banned guid's..\n";
//
//     if( !resultQuery( query ) ) {
//         std::cout << "\e[0;33m[!]no banned guids to dump to file\e[0m \n";
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
//         std::cout << "[-]deleting old banlist dump file..\n";
//         if ( !system( "rm backup/Banlist.backup" ) )
//             std::cout << "\e[0;33m[!]deleted old backup file..creating new one..\e[0m \n";
//         else {
//             std::cout << "\e[0;31m [ERR]couldn't delete old dump file!\e[0m \n";
//             return;
//         }
//     }
//
//     //backup file ready, preparation of info to dump to file
//     std::vector< std::string > bannedIds = extractData( "select id from banned;" );
//     std::vector< std::string > dataToDump;    // banned player info
//     std::vector< std::string > guidsToDump;   //banned player's guids
//     std::string auxQuery;
//
//     if( bannedIds.empty() ) {   //no need to go ahead, no data to dump!
//         std::cout << "\e[0;31 Db::dumpBannedToFile no banned id's to dump\e[0m \n";
//         *(m_options->log) << "Db::dumpBannedToFile no banned id's to dump \n";
//         return;
//     }
//
//     //use logger class to save info to file
//     Logger *output = new Logger( "backup/Banlist.backup" );
//
//     if ( !output->open() ) {
//         std::cout << "\e[1;31mDb::dumpBannedToFile [ERR] can't write Banlist backup file!\e[0m \n";
//         *(m_options->log) << "Db::dumpBannedToFile [ERR] can't write Banlist backup file!\n";
//         *(m_options->errors) << "[ERR] On " << (*m_options)[m_options->serverNumber].name()  << " : can't write Banlist backup file!\n";
//         return;
//     }
//
//     *output << "#\n# THIS IS A BACKUP FILE OF BANLIST FOUND IN cfg/\n#\n";
//     //here i actually write to file
//     for( unsigned int i = 0; i < bannedIds.size(); i++ ) {
//
//         #ifdef DB_DEBUG   //stamps std::vector id's
//             std::cout<< "id " << i << " " << bannedIds[i] <<std::endl;
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
//             std::cout << "banned user data being dumped " << dataToDump[j] << std::endl;
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
//             std::cout << "banned user guids being dumped " << guidsToDump[k] << std::endl;
//         #endif
//         }
//         *output << "####\n";
//     }
//     *output << "#end";
//
//     std::cout << "\e[0;32m[OK] successfully written Banlist.backup file in 'backup/'\e[0m \n";
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
    for (unsigned int i = 0; i < m_options->size()/*how many servers i've got*/; i++) {
        //create copy command

        //get current database directory
        std::string source((*m_options)[i].dbFolder());

        if (source[source.length()-1 ] != '/') {
            source.append("/");
        }

        source.append(DB_NAME);

        /*//get backup directory
        cmd.append( (*m_options)[i].backupDir() );

        if( cmd[ cmd.length()-1 ] != '/' )
            cmd.append( "/" );*/

        std::string dest((*m_options)[i].dbFolder());

        if (dest[dest.length()-1] != '/') {
            dest.append("/");
        }

        dest.append(DB_NAME);
        dest.append(".backup-");
        dest.append((*m_options)[i].name());
        //cmd.append( timeStamp() );  //add day,month and year to backup file name

        //if( system( cmd.c_str() ) ) {  // 0 is success, if I enter here, cmd FAILED
        if (!copyFile(source, dest)) {
            std::cout << "\e[1;31mDb::dumpDatabase " << source << " to " << dest << " database dump failed!\e[0m \n";
            *(m_options->errors) << "On " << (*m_options)[m_options->serverNumber].name()  << " : Db::dumpDatabase " << source << " to " << dest << " database dump failed!\n";
            return;
        } else {
            //just log this call, no need for output
            *(m_options->errors) << "Db::dumpDatabase SUCCESS, dumped the database\n";

            #ifdef DB_DEBUG
                *(m_options->errors) << "Checking the file...";
                if (handyFunctions::fileOrDirExistance(dest)) {
                    *(m_options->errors) << "It exists!\n";
                } else {
                    *(m_options->errors) << "Fail! Not found!\n";
                }
            #endif
        }
    }
}


/********************************
*         BAN  METHODS          *
********************************/
bool Db::ban(const std::string &nick, const std::string &ip, const std::string &date, const std::string &time, const std::string &guid, const std::string &motive, const std::string &adminGuid)
{
    if (checkBanGuid(guid)) {
        std::cout << "\e[0;33m[!]guid: " << guid << " already banned\e[0m \n";
        *(m_options->log) << "[!]guid: " << guid << " already banned\n";
        return false;
    }

    std::string adminNick = handyFunctions::correggi(getAdminNick(adminGuid));

    if (adminNick.empty()) {
        std::cout << "\e[1;31m[ERROR] Admin nick is empty! Can't find a nick for guid: " << adminGuid << "\e[0m \n";
        *(m_options->log) << "[ERROR] Admin nick is empty! Can't find a nick for guid: " << adminGuid << "\n";
        return false;
    }

    std::string banId = insertNewBanned(nick, ip, date, time, motive, adminNick);    //get the autoincrement id

    if (banId.empty()) {                                // empty std::string is ERROR, didn't write to database
        std::cout << "\e[0;33m[!]RETURNING FALSE \e[0m \n";
        return false;
    } else if (insertNewGuid(guid, banId).empty()) {    // empty std::string is ERROR
        std::cout << "\e[0;33m[!]RETURNING FALSE \e[0m \n";
        return false;
    } else {                                            //all went well, add admin to AUTHOR
        std::cout << "\e[0;33m[!]RETURNING TRUE \e[0m \n";
        return true;
    }
}


std::string Db::insertNewBanned(const std::string& nick, const std::string& ip, const std::string& date, const std::string &time, const std::string &motive, std::string adminNick)
{
    std::string newBanQuery("insert into banned( nick, ip, date, time, motive, author ) values('");

    if (adminNick.empty()) {   //if no author, set it to BanBot
        adminNick.clear();
        adminNick = "BanBot";
    }

    newBanQuery.append(nick);
    newBanQuery.append("','");
    newBanQuery.append(ip);
    newBanQuery.append("','");
    newBanQuery.append(date);
    newBanQuery.append("','");
    newBanQuery.append(time);
    newBanQuery.append("','");
    newBanQuery.append(motive);
    newBanQuery.append("','");
    newBanQuery.append(adminNick);
    newBanQuery.append("');");

    if (!execQuery(newBanQuery)) {
        return std::string();  //FAIL return empty std::string
    }

    //else return last banId
    std::string selectLastInserted("select id from banned where nick='");
    selectLastInserted.append(nick);
    selectLastInserted.append("' and ip='");
    selectLastInserted.append(ip);
    selectLastInserted.append("';");

#ifdef DB_DEBUG
    std::cout << "Db::insertNewBanned : selectLastInserted query ->  " << selectLastInserted << std::endl;
#endif

    /* get max */
    if (execQuery(selectLastInserted)) {
#ifdef DB_DEBUG
        std::cout << "SHOWING VDATA\n";
        for (unsigned int i = 0; i < m_data.size(); ++i) {
            std::cout << m_data[i] << "\n";
        }
#endif
        return m_data[0];   // @ 0 we have the id
    } else {
        return std::string();
    }
}


bool Db::modifyBanned(const std::string &nick, const std::string &ip, const std::string &date, const std::string &time, const std::string &motive, const std::string &id)
{
    std::string query("update banned set ");
    bool paramCount = false;
    //TODO semplificare questa funzione usando un std::vector o struct con chiave-valore

    if (!nick.empty()) {
        query.append("nick = '");
        query.append(nick);
        query.append("' ");
        paramCount = true;
    }

    if (!ip.empty()) {
        if (paramCount) {
            query.append(",");
        }
        query.append("ip = '");
        query.append(ip);
        query.append("' ");
        paramCount = true;
    }

    if (!date.empty()) {
        if (paramCount) {
            query.append(",");
        }
        query.append("date = '");
        query.append(date);
        query.append("' ");
        paramCount = true;
    }

    if (!time.empty()) {
        if (paramCount) {
            query.append(",");
        }
        query.append("time = '");
        query.append(time);
        query.append("' ");
        paramCount = true;
    }

    if (!motive.empty()) {
        if (paramCount) {
            query.append(",");
        }
        query.append("motive = '");
        query.append(motive);
        query.append("' ");
    }

    query.append("where id = '");
    query.append(id);
    query.append("';");

    //std::cout << "\nmodify banned" << query << "\n"
    if (!execQuery(query)) {
        std::cout << "\e[0;31m[FAIL] Db::modifyBanned : " << query << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::modifyBanned : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::modifyBanned : " << query << "\n";
        return false;
    } else {
        return true;
    }
}


bool Db::deleteBanned(const std::string &id)
{
    if (id.empty()) {
        std::cout << "\e[0;31m Db::deleteBanned empty id! \e[0m \n";
        return false;
    }

    //first clean guid records
    std::string deleteGuidsQuery("delete from guids where banId='");
    deleteGuidsQuery.append(id);
    deleteGuidsQuery.append("';");

#ifdef DB_DEBUG
    std::cout << "Db::deleteBanned guids query -> " << deleteGuidsQuery << "\n";
#endif

    if (!execQuery(deleteGuidsQuery)) {
        std::cout << "\e[0;31m[FAIL] Db::deleteBanned can't delete guid with banId = " << id << "\e[0m \n ";
        *(m_options->log) << "[FAIL] Db::deleteBanned can't delete guid with banId = " << id << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::deleteBanned can't delete guid with banId = " << id << "\n";
        return false;
    }

    std::cout << "\e[0;31m[!] deleted all guids with ban id = " << id << "\e[0m \n";
    *(m_options->log) << "[!] deleted all guids with ban id = " << id << "\n";


    //and now clean banned table
    std::string query("delete from banned where id ='");
    query.append(id);
    query.append("';");

#ifdef DB_DEBUG
    std::cout << "Db::deleteBanned bantable query -> " << query << "\n";
#endif

    if (!execQuery(query)) {
        std::cout << "\e[0;31m[FAIL] Db::deleteBanned : " << query << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::deleteBanned : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::deleteBanned : " << query << "\n";
        return false;
    }

    return true;
}


/********************************
*      GUID TABLE METHODS       *
********************************/
std::string Db::insertNewGuid(const std::string& guid, const std::string &banId)
{
    std::string newGuidQuery("insert into guids( guid, banId ) values('");

    newGuidQuery.append(guid);
    newGuidQuery.append("','");
    newGuidQuery.append(banId);
    newGuidQuery.append("');");

    if(!execQuery(newGuidQuery)) {
        return std::string();  /* FAIL return empty std::string */
    }

    // else return last banId
    if (execQuery("select max( id ) from guids")) {
        return m_data[0];
    } else {
        return std::string();
    }
}


bool Db::modifyGuid(const std::string& guid, const std::string &banId, const std::string &id)
{
    std::string query("update guids set ");
    bool paramCount = false;

    if (!guid.empty()) {
        query.append("guid = '");
        query.append(guid);
        query.append("' ");
        paramCount = true;
    }

    if (!banId.empty()) {
        if (paramCount) {
            query.append(",");
        }
        query.append("banId = '");
        query.append(banId);
        query.append("' ");
    }

    query.append("where id = '");
    query.append(id);
    query.append("';");

    if (!execQuery(query)) {
        std::cout << "\e[0;31m[FAIL] Db::modifyGuid : " << query << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::modifyGuid : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::modifyGuid : " << query << "\n";
        return false;
    } else {
        return true;
    }
}


bool Db::deleteGuid(const std::string &id)
{
    std::string query("delete from guids where id ='");
    query.append(id);
    query.append("';");

    if (!execQuery(query)) {
        std::cout << "\e[0;31m[FAIL] Db::deleteGuid : " << query << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::deleteGuid : " << query << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::deleteGuid : " << query << "\n";
        return false;
    } else {
        return true;
    }
}


/********************************
*     OPLIST TABLE METHODS      *
********************************/
bool Db::addOp(const std::string& nick, const std::string& guid, const std::string &opLvl)
{
    if (checkAuthGuid(guid) < 100) {
        std::cout << "\e[0;33m[!]Admin: " << nick << " : " << guid << " already exists on Database\e[0m \n";
        *(m_options->log) << "[!]Admin: " << nick << " : " << guid << " already exists on Database\n";
        return false;
    }

    std::string addOpQuery("insert into oplist( nick, guid, level ) values('");
    addOpQuery.append(nick);
    addOpQuery.append("','");
    addOpQuery.append(guid);
    addOpQuery.append("','");
    addOpQuery.append(opLvl);
    addOpQuery.append("');");

    return execQuery(addOpQuery); //true on success
}


bool Db::modifyOp(const std::string& id, const std::string& nick, const std::string& guid, const std::string &opLvl)
{
    std::string modifyQuery("update oplist set ");
    bool paramCount = false;

    if (!nick.empty()) {
        modifyQuery.append("nick = '");
        modifyQuery.append(nick);
        modifyQuery.append("' ");
        paramCount = true;
    }

    if (!guid.empty()) {
        if(paramCount) {
            modifyQuery.append(",");
        }
        modifyQuery.append("guid = '");
        modifyQuery.append(guid);
        modifyQuery.append("' ");
        paramCount = true;
    }

    if (!opLvl.empty()) {
        if (paramCount) {
            modifyQuery.append(",");
        }
        modifyQuery.append("level='");
        modifyQuery.append(opLvl);
        modifyQuery.append("' ");
    }

    modifyQuery.append("where id = '");
    modifyQuery.append(id);
    modifyQuery.append("';");

    if (!execQuery( modifyQuery)) {
        std::cout << "\e[0;31m[FAIL] Db::modifyOp : " << modifyQuery << "\e[0m \n";
        *(m_options->log) << "[FAIL] Db::modifyOp : " << modifyQuery << "\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::modifyOp : " << modifyQuery << "\n";
        return false;
    } else {
        return true;
    }
}


bool Db::deleteOp(const std::string& id)
{
    std::string deleteQuery("delete from oplist where id='");
    deleteQuery.append(id);
    deleteQuery.append("';");

    return execQuery(deleteQuery);
}


/********************************
* CUSTOM QUERIES FOR ANALYZER   *
********************************/

/* id motive queries */
std::vector<Db::idMotiveStruct> Db::idMotiveViaGuid(const std::string& guid)
{
    std::string query("SELECT banned.id,banned.motive,banned.date,banned.time FROM banned join guids ON banned.id=guids.banId WHERE guids.guid='");
    query.append(guid);
    query.append("' order by banned.date DESC, banned.time DESC;");

    std::vector<idMotiveStruct>structs;

    if (!execQuery(query)) {   /* on fail, return immediatly empty struct */
        return structs;
    }

    #ifdef DB_DEBUG
    std::cout << "Db::idMotiveViaGuid\n";
    for (unsigned int i = 0; i < m_data.size(); i++) {
        std::cout << "ANSWER" << i << " is -> " << m_data.at(i) << std::endl;
    }
    #endif

    if (m_data.size() > 0) {
        structs.push_back(idMotiveStruct(m_data[0], m_data[1], m_data[2], m_data[3]));  /* insert in order, id, motive, date, time */
    }

    return structs;
}

std::vector<Db::idMotiveStruct> Db::idMotiveViaIp(const std::string& ip)
{
    std::string query("SELECT id,motive,date,time FROM banned WHERE ip='");
    query.append(ip);
    query.append("' order by date DESC, time DESC;");

    std::vector<idMotiveStruct>structs;

    if (!execQuery(query)) {
        return structs; /* on fail, return immediatly empty struct */
    }

    #ifdef DB_DEBUG
    std::cout << "Db::idMotiveViaIp" << std::endl;

    for (unsigned int i = 0; i < m_data.size(); i++) {
        std::cout << "ANSWER " << i << " is -> " << m_data.at(i);
    }
    #endif
    //  SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW
    if (m_data.size() > 0) {
        structs.push_back(idMotiveStruct(m_data[0], m_data[1], m_data[2], m_data[3]));  /* insert in order, id, motive, date, time */
    }

    return structs;
}

std::vector<Db::idMotiveStruct> Db::idMotiveViaNick(const std::string& nick)
{

    std::string query("select id,motive,date,time from banned where nick='");
    query.append(nick);
    query.append("' order by date DESC, time DESC;");

    std::vector<idMotiveStruct>structs;

    if (!execQuery(query)) {
        return structs; /* on fail, return immediatly empty struct */
    }


    #ifdef DB_DEBUG
    std::cout << "Db::idMotiveViaNick" << std::endl;

    for (unsigned int i = 0; i < m_data.size(); i++) {
        std::cout << "ANSWER " << i << " is -> " << m_data.at(i);
    }
    #endif
    //  SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW
    if (m_data.size() > 0) {
        structs.push_back(idMotiveStruct(m_data[0], m_data[1], m_data[2], m_data[3]));  /* insert in order, id, motive, date, time */
    }

    return structs;
}

/* "how many" queries */
std::string Db::autoBanned()
{
    std::string query("select count(*) from banned where author='BanBot' group by id;");

    #ifdef DB_DEBUG
    std::cout << "Db::autoBanned returning value " << resultQuery(query) << std::endl;
    #endif

    return handyFunctions::intToString(resultQuery(query));
}

std::string Db::banned()
{
    std::string query("select count(*) from banned group by id;");

    #ifdef DB_DEBUG
    std::cout << "Db::banned returning value " << resultQuery(query) << std::endl;
    #endif

    return handyFunctions::intToString(resultQuery(query));
}

std::string Db::ops()
{
    std::string query("select count(*) from oplist group by id;");

    #ifdef DB_DEBUG
    std::cout << "Db::ops returning value " << resultQuery(query) << std::endl;
    #endif

    return handyFunctions::intToString(resultQuery(query));
}

Db::idNickStruct Db::opStruct(const std::string &id)
{
    std::string query("select nick, level from oplist where id='" + id + "';");
    
    if (execQuery(query) && !m_data.empty()) {
        return idNickStruct(id, m_data[0], m_data[1]);
    }
    // else return empty admin
    return idNickStruct();
}

std::string Db::adminRegisteredNickViaGuid(const std::string& guid)
{
    std::string query("select nick from oplist where guid='");
    query.append(guid);
    query.append("';");

    if (execQuery(query) && !m_data.empty()) {
        return m_data[0];
    }
    // in both failed cases i return an empty std::string
    return std::string();
}


/* find queries */
std::vector<Db::idNickMotiveAuthorStruct> Db::findAproxIdMotiveAuthorViaNickBanned(const std::string& nick)
{
    std::string query("select id,nick,motive,author from banned where nick like '%");
    query.append(nick);
    query.append("%' limit 16;");

    std::vector<idNickMotiveAuthorStruct> structs;

    if (!execQuery(query)) {
        return structs;  /* on fail, return immediatly empty struct */
    }

    #ifdef DB_DEBUG
    std::cout << "Db::findAproxIdMotiveAuthorViaNickBanned" << std::endl;
    std::cout << "mdata SIZE IS -> " << m_data.size() << std::endl;

    for (unsigned int i = 0; i < m_data.size(); i++) {
        std::cout << "ANSWER " << i << " is -> " << m_data.at(i);
    }
    #endif

    for (unsigned int i = 0; i < m_data.size()/4; i++) {
        /// TODO still need this output?
        std::cout << "TEST for segfault\n";
        structs.push_back(idNickMotiveAuthorStruct(m_data.at(4*i + 0), m_data.at(4*i + 1), m_data.at(4*i + 2), m_data.at(4*i + 3)));
    }

    return structs;
}

std::vector<Db::idNickMotiveAuthorStruct> Db::findPreciseIdMotiveAuthorViaNickBanned(const std::string &nick)
{
    std::string query("select id,nick,motive,author from banned where nick='");
    query.append(nick);
    query.append("' limit 7;");

    std::vector<idNickMotiveAuthorStruct> structs;

    if (!execQuery(query)) {
        return structs; /* on fail, return immediatly empty struct */
    }

    #ifdef DB_DEBUG
    std::cout << "Db::findPreciseIdMotiveAuthorViaNick" << std::endl;

    for (unsigned int i = 0; i < m_data.size(); i++) {
        std::cout << "ANSWER " << i << " is -> " << m_data.at(i);
    }
    #endif

    std::cout << "ANSWER SIZE IS -> " << m_data.size() << std::endl;

    /* SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW */
    if (m_data.size() > 0) {
        structs.push_back(idNickMotiveAuthorStruct(m_data[0], m_data[1], m_data[2], m_data[3]));    /* insert in order, id, nick, motive, author */
    }

    return structs;
}

std::vector<Db::idNickStruct> Db::findAproxIdNickViaNickOp(const std::string& nick)
{
    std::string query("select id,nick,level from oplist where nick like '%");
    query.append(nick);
    query.append("%' limit 16;");

    std::vector<idNickStruct> structs;

    if (!execQuery(query)) {
        return structs;     /* on fail, return immediatly empty struct */
    }

    #ifdef DB_DEBUG
    std::cout << "Db::findAproxIdNickViaNickOp" << std::endl;

    for (unsigned int i = 0; i < m_data.size(); i++) {
        std::cout << "ANSWER " << i << " is -> " << m_data.at(i);
    }
    #endif

    for (unsigned int i = 0; i < m_data.size()/3; i++) {
        structs.push_back(idNickStruct(m_data[3*i + 0], m_data[3*i + 1], m_data[3*i + 2]));
    }

    return structs;
}

std::vector<Db::idNickStruct> Db::findPreciseIdNickViaNickOp(const std::string& nick)
{
    std::string query("select id,nick from oplist where nick='");
    query.append(nick);
    query.append("' limit 7;");

    std::vector<idNickStruct> structs;

    if (!execQuery(query)) {
        return structs;     /* on fail, return immediatly empty struct */
    }

    #ifdef DB_DEBUG
    std::cout << "Db::findPreciseIdNickViaNickOp" << std::endl;

    for (unsigned int i = 0; i < m_data.size(); i++) {
        std::cout << "ANSWER " << i << " is -> " << m_data.at(i);
    }
    #endif

    /* SHOULD I ASSUME THAT THERE ARE NO DOUBLES?  YES FOR NOW */
    if (m_data.size() > 0) {
        structs.push_back(idNickStruct(m_data[0], m_data[1], m_data[2]));
    }

    return structs;
}


/*******************************
*     SPECIAL METHODS          *
*******************************/

bool Db::sequentialDbUpgrade()
{
    std::cout<<"********* Starting the upgrade: ***********\n";
    m_options->errors->timestamp();
    *(m_options->errors)<<"********* Starting the upgrade: ***********\n";
    for (unsigned int i = 0; i < m_options->size(); i++) {
        //for each db, i'll check the version
        std::cout << "Starting version of database in \"" << (*m_options)[i].dbFolder() << "\": ";
        m_options->serverNumber = i;
        *(m_options->errors) << "Starting version of database in \""<< (*m_options)[i].dbFolder() << "\": ";
        openDatabase();
        DbVersion version = checkDbVersion();
        std::cout << version << "\n";
        *(m_options->errors) << version << "\n";
        if (version == VER_1_1) {
            std::cout<<"converting it to " << VER_1_2 << "\n";
            *(m_options->errors) << "converting it to " << VER_1_2 << "\n";
            if (execQuery("ALTER TABLE oplist ADD COLUMN level TEXT DEFAULT '0';")) {
                *(m_options->errors) << "Ok, converted. ";
                std::cout << "Ok, converted. ";
            } else {
                std::cout << "ERROR converting.\n";
                *(m_options->errors) << "ERROR converting.\n";
                return false;
            }
            version = checkDbVersion();
            std::cout << "New version: " << version << "\n";
            *(m_options->errors) << "New version: " << version << "\n";
        }
    }

    m_options->serverNumber = 0;
    return true;
}

/********************************
*       PRIVATE METHODS         *
********************************/
Db::DbVersion Db::checkDbVersion()
{
    if (checkForDbVersion1_1()) {
        return VER_1_1;
    } else if (checkForDbVersion1_2()) {
        return VER_1_2;
    } else {
        return UNKOWN;
    }
}

bool Db::checkForDbVersion1_1()
{
    bool banFlag = false
    , oplistFlag = false
    , guidsFlag = false;

    // banned table check
    if (execQuery("select sql from sqlite_master where name='banned';")) {
        if (!m_data.empty()) {
            if (m_data[0].compare(BANNED_1_1) == 0) {
                banFlag = true;
            }
        }
    }

    // oplist table check
    if (execQuery("select sql from sqlite_master where name='oplist';")) {
        if (!m_data.empty()) {
            if (m_data[0].compare(OPLIST_1_1) == 0) {
                oplistFlag = true;
            }
        }
    }

    // banned table check
    if (execQuery("select sql from sqlite_master where name='guids';")) {
        if (!m_data.empty()) {
            if (m_data[0].compare(GUIDS_1_1) == 0) {
                guidsFlag = true;
            }
        }
    }

    if (!banFlag || !oplistFlag || !guidsFlag) {
        return false;
    } else {
        return true;
    }
}

bool Db::checkForDbVersion1_2()
{
    bool banFlag = false
    , oplistFlag = false
    , guidsFlag = false;

    // banned table check
    if (execQuery("select sql from sqlite_master where name='banned';")) {
        if (!m_data.empty()) {
            if (m_data[0].compare(BANNED_1_2) == 0) {
                banFlag = true;
            }
        }
    }

    // oplist table check
    if (execQuery("select sql from sqlite_master where name='oplist';")) {
        if (!m_data.empty()) {
            if (m_data[0].compare(OPLIST_1_2) == 0) {
                oplistFlag = true;
            }
        }
    }

    // banned table check
    if (execQuery("select sql from sqlite_master where name='guids';")) {
        if (!m_data.empty()) {
            if (m_data[0].compare(GUIDS_1_2) == 0) {
                guidsFlag = true;
            }
        }
    }

    if (!banFlag || !oplistFlag || !guidsFlag) {
        return false;
    } else {
        return true;
    }
}

bool Db::connect()  //called by Db::open ( public function )
{
    std::cout << "\e[0;33m connecting to database in " << (*m_options)[m_options->serverNumber].dbFolder() << "\e[0m \n";
    //get path to current db
    std::string database((*m_options)[m_options->serverNumber].dbFolder());

    if (database[database.size()-1] != '/') {  //CAREFUL, NEED THIS!!!
        database.append("/");
    }

    database.append(DB_NAME);             // and now i have database full path + name

    if (sqlite3_open(database.c_str(), &m_database)) {
        std::cout<<"\e[0;31m[EPIC FAIL] " << sqlite3_errmsg(m_database) << "\e[0m \n";
        *(m_options->log) <<"[FAIL] " << sqlite3_errmsg(m_database);
        *(m_options->errors) <<"[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << "@ " << database << " : " << sqlite3_errmsg(m_database);
        sqlite3_close(m_database);
        return false;
    }

#ifdef DB_DEBUG
    std::cout << "\e[1;35mOPENED DB\e[0m \n";
    std::cout << "\e[1;35musing database in " << (*m_options)[m_options->serverNumber].dbFolder() << " \e[0m \n";
#endif

    return true;
}

bool Db::copyFile(const std::string &source, const std::string &destination)
{
    std::ifstream src;   // the source file
    std::ofstream dest;  // the destination file

    src.open(source.c_str(), std::ios::binary);   // open in binary to prevent jargon at the end of the buffer
    dest.open(destination.c_str(), std::ios::binary);  // same again, binary

    if (!src.is_open() || !dest.is_open()) {
        return false;       // could not be copied
    }

    dest << src.rdbuf ();   // copy the content
    dest.close ();          // close destination file
    src.close ();           // close source file

    return true;            // file copied successfully
}


void Db::createDb()   //initial creation of database
{
    //checks...
    // ALWAYS USE LATEST VERSION!
    if (resultQuery(BANNED_1_2) == 0) {
        std::cout << "\e[0;32m    [*]created 'banned' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'banned' table..\n";
    } else {
        std::cout << "\e[0;31m    [FAIL]error creating 'banned' table on database @ : " << (*m_options)[m_options->serverNumber].name() << "\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'banned' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'banned' table\n";
    }

    if (resultQuery(GUIDS_1_2) == 0) {
        std::cout << "\e[0;32m     [*]created 'guid' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'guid' table..\n";
    } else {
        std::cout << "\e[0;31m    [FAIL]error creating 'guid' table on database @ : " << (*m_options)[m_options->serverNumber].name() << "\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'guid' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'guid' table\n";
    }

    if (resultQuery(OPLIST_1_2) == 0) {
        std::cout << "\e[0;32m     [*]created 'oplist' table..\e[0m \n";
        *(m_options->log) << "    [*]created 'oplist' table..\n";
    } else {
        std::cout << "\e[0;31m    [FAIL]error creating 'oplist' table on database @ : " << (*m_options)[m_options->serverNumber].name() << "\e[0m \n";
        *(m_options->log) << "    [FAIL]error creating 'oplist' table\n";
        *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : error creating 'oplist' table\n";
    }
}


bool Db::execQuery(const std::string &query)
{

#ifdef DB_DEBUG
    std::cout << "Db::execQuery - " << query << "\n";
    std::cout << "using database @: " << (*m_options)[m_options->serverNumber].dbFolder() << std::endl;
#endif

    // reset result occurences
    m_resultOccurrences = 0;

    //clean old m_data... MUST clear otherwise i keep old values
    if (!m_data.empty()) {
        m_data.clear();
    }

    //..and old m_vcolHead just to be shure i don't get unwanted info
    if (!m_vcolHead.empty()) {
        m_vcolHead.clear();
    }

    //and also check that database connection is open!
    if (!m_database) {
        connect();
    }

    m_resultCode = sqlite3_get_table(m_database, query.c_str(), &m_result, &m_nrow, &m_ncol, &m_zErrMsg);
    bool success;

    if (m_vcolHead.size() < 0) {
        m_vcolHead.clear();
    }

    if (m_data.size() < 0) {
        m_data.clear();
    }

    if (m_resultCode == SQLITE_OK) {   // went well and got data
        success = true;

        for (int i = 0; i < m_ncol; ++i) {
            m_vcolHead.push_back(m_result[i]);   // First row heading
            for (int i = 0; i < m_ncol * m_nrow; ++i) {
                m_data.push_back( m_result[m_ncol+i] );
            }
        }

    #ifdef DB_DEBUG
        for (unsigned int i = 0; i < m_data.size(); ++i) {
            std::cout << "M_DATA INFO\n";
            std::cout << m_data[i] << "\n";
        }
    #endif

        // set result occurences
        m_resultOccurrences = (int)m_data.size();

    } else {
        success = false;
        std::cout << "\e[1;31m[EPIC FAIL] Db::execQuery " << sqlite3_errmsg(m_database) << "\e[0m \n";
        *(m_options->log) << "[EPIC FAIL] Db::execQuery '" << m_zErrMsg << "'\n";
        *(m_options->log) << "[ERROR MSG]" << sqlite3_errmsg(m_database) << "\n";
        *(m_options->errors) << "[EPIC FAIL] On " << (*m_options)[m_options->serverNumber].name()  << " : Db::execQuery '" << m_zErrMsg << "'\n";
        *(m_options->errors) << sqlite3_errmsg(m_database);
    }
    sqlite3_free_table(m_result);

#ifdef DB_DEBUG
    std::cout << "\e[1;37m Db::execQuery result code is: " << m_resultCode << " sending value: " << success << "\e[0m \n";
#endif
    return success;
}


std::string Db::getAdminNick(const std::string& guid)
{
    //if empty return empty std::string and don't do query
    //otherwise it wil crash returning vdata[0] because it doesn't exist!
    if (guid.empty()) {
        return std::string();
    }

    std::string query("select nick from oplist where guid='");
    query.append(guid);
    query.append("';");

    if (execQuery(query) && m_data.size() > 0) {
        return m_data[0];
    } else {
        return std::string();
    }
}


// void Db::loadAdminlist( std::vector< ConfigLoader::AdminList > admins )
// {
//     std::cout << "\e[0;33m[!] Loading admins to database.. \e[0m \n";
//
//     if( admins.empty() ) {
//         std::cout<<"\e[0;33m[!]Adminlist EMPTY\e[0m \n";
//         *(m_options->log)<<"[!]Adminlist EMPTY\n";
//         return;
//     }
//
//     int addedCounter = 0;
//
//     //adding admins to db
//     std::cout<<"\n[-]Adding admins to database..\n\n";
//
// #ifdef DB_DEBUG
//     std::cout << "ADMIN SIZE-> " << admins.size() << "\n";
// #endif
//
//     for( unsigned int i = 0; i < admins.size(); i++ ) {
//
// //        addop already checks for guid on database. Don't need to do check here!
// //        if( !checkAuthGuid( admins[i].value ) ) {    //non esiste sul database
// //            //add to database
//         if ( addOp( admins[i].name, admins[i].value ) ) {
//             std::cout << "\e[0;32m      [+]added admin: " << admins[i].name << "\e[0m \n";
//             *(m_options->log)<<"      [+]added admin: " << admins[i].value << "\n";
//             addedCounter++;
//         }
//     }
//     std::cout << "\e[0;33m Added " << addedCounter << "/" << admins.size() << " new admin/s from file to the database\e[0m \n\n";
//     *(m_options->log) << "Added " << addedCounter << " new admin/s from file to the database\n";
// }
//
//
// void Db::loadBanlist( std::vector<ConfigLoader::Banlist> banned )
// {
//     #ifdef DB_DEBUG
//     std::cout << "Db::loadBanlist\n";
//     #endif
//
//     std::cout << "\e[0;33m[!] Loading banlist to database.. \e[0m \n";
//
//     if( banned.empty() ) {
//         std::cout<<"\e[0;33m[!]Banlist EMPTY\e[0m \n";
//         *(m_options->log)<<"[!]Banlist EMPTY\n";
//         return;
//     }
//
//     int playerCounter = 0,/* onDbCounter = 0,*/ addedGuidsCounter = 0;
//
//     //adding banned users to db
//     std::cout<<"\n[-]Adding banned users to database..\n\n";
//
//     for( unsigned int i = 0; i < banned.size() - 1; i++ ) {  //TODO find out why it works with "-1" while for the guids i don't need the "-1"
//
//         if( !checkBanNick( banned[i].nick ) ) {   //if not on database
//
//         #ifdef DB_DEBUG
//             std::cout << "Db::loadAdminlist " << banned[i].nick << " not on database!\n";
//             std::cout << "Db::loadBanlist ban info -> " << banned[i].nick << " " << banned[i].ip << " " << banned[i].date << " " << banned[i].time << " " << banned[i].motive << " " << banned[i].author << std::endl;
//         #endif
//
//             std::string banId = insertNewBanned( banned[i].nick, banned[i].ip, banned[i].date, banned[i].time, banned[i].motive, banned[i].author );
//
//             if( banId.empty() ) {
//                 std::cout << "\e[0;31m[FAIL] can't add banned player: " << banned[i].nick << " to banned database!\e[0m \n";
//                 *(m_options->log) << "[FAIL] can't add banned player: " << banned[i].nick << " to banned database!\n";
//                 *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].getName()  << " : can't add banned player: " << banned[i].nick << " to banned database!\n";
//                 return;
//             }
//
//             playerCounter++; //one user added
//
//             for( unsigned int j = 0; j < banned[i].guids.size(); j++ ) {
//                 std::string guidId = insertNewGuid( banned[i].guids[j], banId );
//
//                 if( guidId.empty() ) {
//                     std::cout << "\e[0;31m[FAIL] can't add banned player's guid: " << banned[i].nick << " to database!\e[0m \n";
//                     *(m_options->log) << "[FAIL] can't add banned player's guid: " << banned[i].nick << " to database!\n";
//                     *(m_options->errors) << "[FAIL] On " << (*m_options)[m_options->serverNumber].getName()  << " : can't add banned player's guid: " << banned[i].nick << " to banned database!\n";
//                     return;
//                 }
//                 addedGuidsCounter++;
//             }
//         }
//     #ifdef DB_DEBUG
//         else
//             std::cout << banned[i].nick << " on database!\n";
//     #endif
//
//     }
//     std::cout << "\e[0;33m Added " << playerCounter << " clients and " << addedGuidsCounter << " new guids to the database\e[0m \n";
//     *(m_options->log) << "Added " << playerCounter << " clients and " << addedGuidsCounter << " new guids to the database \n";
// }


int Db::resultQuery(const std::string &query) //ritorna quante corrispondenze ci sono all'interno del DB
{

#ifdef DB_DEBUG
    std::cout << "Db::resultQuery\n";
    std::cout << "query to check : " << query << "\n";
#endif

    int answer = 0;

    if (!execQuery(query)) {
        std::cout << "query failed\n";
        answer = -1;
    } else {
        answer = m_resultOccurrences;
    }

#ifdef DB_DEBUG
    std::cout << "\e[1;37mDb::resultQuery ANSWER SIZE : " << m_resultOccurrences << "\e[0m \n";
    std::cout << "\e[1;37mDb::resultQuery ANSWER SENDING OUT IS : " << answer << "\e[0m \n";
#endif

    return answer;
}

