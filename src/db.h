/*
    db.h is part of BanBot.

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


#ifndef DB_H
#define DB_H

#include <iostream>
#include <string>
#include <vector>
#include "ConfigLoader.h"
#include "sqlite3/sqlite3.h"

#define DB_NAME "Db.sqlite"

class Logger;

// Class used by Analyzer for interaction with sqlite3 database where the bot stores it's information
// useful to keep track of banned players and admins that have power over it

class Db
{
    public:
        Db( ConfigLoader::Options* conf );
        ~Db();

        // database connection
        bool openDatabase();    // opens a connection with the sqlite3 database.\nReturns true if connection succeded else returns false for failiure
        void closeDatabase();   // closes database connection

        // checks
        int checkAuthGuid( const std::string &guid );   // checks for the existance of "guid" in the oplist table (returns the op level)
        bool checkBanGuid( const std::string &guid );    // checks if "guid" is present amongst the banned users
        bool checkBanNick( const std::string &nick );    // checks if "nick" is present amongst the banned users
        void checkDatabases();                      // creates and checks if database files exist and flags them in m_options
        bool isOpTableEmpty();                      //checks if oplist table is empty ( returns true ) else returns false
        //bool checkDirAndFile( const std::string &guid ); //passa url file compreso

        // dumps
//        void dumpAdminsToFile();    /*!< dumps admins to file ( backup/Adminlist.backup )*/
//        void dumpBannedToFile();    /*!< dumps banned users to file ( backup/Banlist.backup )*/
        void dumpDatabases();                       // dumps database creating a copy

        //banned table
        bool ban( const std::string &nick, const std::string &ip, const std::string &date, const std::string &time, const std::string &guid, const std::string &motive, const std::string &adminGuid );    // bans player
        std::string insertNewBanned( const std::string &nick, const std::string &ip,  const std::string &date, const std::string &time, const std::string &motive, std::string adminNick );                // insert banned player info into "banned" table
        bool modifyBanned( const std::string &nick, const std::string &ip,  const std::string &date, const std::string &time, const std::string &motive, const std::string &id );                     // modify info for banned player with id = "id"
        bool deleteBanned( const std::string &id );                                                                                                                          // deletes records for banned player with given "id"

        // guid table
        std::string insertNewGuid( const std::string &guid, const std::string &banId );                // inserts guid with specified "banId" to records
        bool modifyGuid( const std::string &guid, const std::string &banId, const std::string &id );   // modifies guid with given "guid" where banId and "id" find a match
        bool deleteGuid( const std::string &id );                                            // deletes records for guid with given "id"

        // oplist table
        bool addOp( const std::string &nick, const std::string &guid, const std::string &opLvl );                       // add's op with given "nick" and "guid" with given lvl
        bool modifyOp( const std::string &id, const std::string &nick, const std::string &guid, const std::string &opLvl );  // modifies op's "nick" and/or "guid" and/or "op level" where "id" finds a match
        bool deleteOp( const std::string &id );                                                               // deletes records for op with given "id"

        //  structs for ANALYZER'S custom queries
        struct idNickStruct
        {
            std::string id, nick, level;

            idNickStruct( const std::string &id, const std::string &nick, const std::string &lvl )
                : id( id )
                , nick( nick )
                , level( lvl )
            {}
        };

        struct idMotiveStruct
        {
            std::string id, motive, date, time;

            idMotiveStruct( const std::string &id, const std::string &motive
                          , const std::string &date, const std::string &time )
                : id( id )
                , motive( motive )
                , date( date )
                , time( time )
            {}
        };

        struct idNickMotiveAuthorStruct
        {
            std::string id, nick, motive, author;

            idNickMotiveAuthorStruct( const std::string &id, const std::string &nick
                                    , const std::string &motive, const std::string &author )
                : id( id )
                , nick( nick )
                , motive( motive )
                , author( author )
            {}
        };

        // id motive
        std::vector< idMotiveStruct > idMotiveViaGuid( const std::string &guid ); // returns id, motive, date and time of a ban by looking up the guid
        std::vector< idMotiveStruct > idMotiveViaIp( const std::string &ip );     // returns id, motive, date and time of a ban by looking up the ip
        std::vector< idMotiveStruct > idMotiveViaNick( const std::string &nick ); // returns id, motive, date and time of a ban by looking up the nick

        /* custom queries for analyzer */
        //"how many?" queries
        std::string autoBanned();                                        // returns how many users have been autobanned by the bot
        std::string banned();                                            // returns how many have been banned
        std::string ops();                                               // returns how many ops are registered to the bot

        std::string adminRegisteredNickViaGuid( const std::string &guid );    // returns the nick which with the admin is registered to the bot

        //"find" queries
        std::vector< idNickMotiveAuthorStruct > findAproxIdMotiveAuthorViaNickBanned( const std::string &nick );      // returns idNickMotiveAuthor struct after aprox find query ( BANNED TABLE )
        std::vector< idNickMotiveAuthorStruct > findPreciseIdMotiveAuthorViaNickBanned( const std::string &nick );    // returns idNickMotiveAuthor struct after precise find query ( BANNED TABLE )

        std::vector< idNickStruct > findAproxIdNickViaNickOp( const std::string &nick );                              //  returns idNick struct after aprox find query ( OPLIST TABLE )
        std::vector< idNickStruct > findPreciseIdNickViaNickOp( const std::string &nick );                            // returns idNick struct after precise find query ( OPLIST TABLE )

        bool sequentialDbUpgrade();                                         // upgrade the db to be conpatible with the new version.

    private:
        enum DbVersion {
            UNKOWN,     // unkown version, probably due to an error
            VER_1_1,    // version 1.1
            VER_1_2     // version 1.2
        };

        DbVersion checkDbVersion();                                         // checks current database version and returns version in use
        bool checkForDbVersion1_1();                                        // checks for database version 1
//         bool checkForDbVersion1_2();                                        // checks for database version 2

        bool connect();                                                     // connects to the sqlite3 database
        bool copyFile( const std::string &source, const std::string &destination );   // copy a file to another one
        void createDb();                                                    // creates database
        std::string errorCodeToString( int errorCode ) const;
        bool execQuery( const std::string &query );                              // executes query and returns status to tell if the operation went well or not
        std::string getAdminNick( const std::string &guid );                          // return's admin's nick from his guid
//         void loadAdminlist( std::vector<ConfigLoader::AdminList> admins );
//         void loadBanlist( std::vector<ConfigLoader::Banlist> banned );
        int resultQuery( const std::string &query );                             //if query fails -> -1 else return number of elements found

        sqlite3 *m_database;
        ConfigLoader::Options* m_options;                                   // where i get all my info from ( bot related )

        char *m_zErrMsg;
        char **m_result;
        int m_resultCode;                                                   // return code i get from query ( use to check query returns. SQLITE_OK ecc )
        int m_nrow, m_ncol;                                                 // number of rows and columns

        // data is stored here after query executions!!
        std::vector< std::string > m_vcolHead;                                        // MUST clear otherwise i keep old values as well
        std::vector< std::string > m_data;                                            // the info i need is stored here after a query
};

#endif //DB_H
