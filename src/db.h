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

using namespace std;

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
        int checkAuthGuid( const string &guid );   // checks for the existance of "guid" in the oplist table (returns the op level)
        bool checkBanGuid( const string &guid );    // checks if "guid" is present amongst the banned users
        bool checkBanNick( const string &nick );    // checks if "nick" is present amongst the banned users
        void checkDatabases();                      // creates and checks if database files exist and flags them in m_options
        bool isOpTableEmpty();                      //checks if oplist table is empty ( returns true ) else returns false
        //bool checkDirAndFile( const string &guid ); //passa url file compreso

        // dumps
//        void dumpAdminsToFile();    /*!< dumps admins to file ( backup/Adminlist.backup )*/
//        void dumpBannedToFile();    /*!< dumps banned users to file ( backup/Banlist.backup )*/
        void dumpDatabases();                       // dumps database creating a copy

        //banned table
        bool ban( const string &nick, const string &ip, const string &date, const string &time, const string &guid, const string &motive, const string &adminGuid );    // bans player
        string insertNewBanned( const string &nick, const string &ip,  const string &date, const string &time, const string &motive, string adminNick );                // insert banned player info into "banned" table
        bool modifyBanned( const string &nick, const string &ip,  const string &date, const string &time, const string &motive, const string &id );                     // modify info for banned player with id = "id"
        bool deleteBanned( const string &id );                                                                                                                          // deletes records for banned player with given "id"

        // guid table
        string insertNewGuid( const string &guid, const string &banId );                // inserts guid with specified "banId" to records
        bool modifyGuid( const string &guid, const string &banId, const string &id );   // modifies guid with given "guid" where banId and "id" find a match
        bool deleteGuid( const string &id );                                            // deletes records for guid with given "id"

        // oplist table
        bool addOp( const string &nick, const string &guid, const string &opLvl );                       // add's op with given "nick" and "guid" with given lvl
        bool modifyOp( const string &id, const string &nick, const string &guid, const string &opLvl );  // modifies op's "nick" and/or "guid" and/or "op level" where "id" finds a match
        bool deleteOp( const string &id );                                                               // deletes records for op with given "id"

        //  structs for ANALYZER'S custom queries
        struct idNickStruct
        {
            string id, nick;

            idNickStruct( const string &id, const string &nick )
                : id( id )
                , nick( nick )
            {}
        };

        struct idMotiveStruct
        {
            string id, motive, date, time;

            idMotiveStruct( const string &id, const string &motive
                          , const string &date, const string &time )
                : id( id )
                , motive( motive )
                , date( date )
                , time( time )
            {}
        };

        struct idNickMotiveAuthorStruct
        {
            string id, nick, motive, author;

            idNickMotiveAuthorStruct( const string &id, const string &nick
                                    , const string &motive, const string &author )
                : id( id )
                , nick( nick )
                , motive( motive )
                , author( author )
            {}
        };

        // id motive
        vector< idMotiveStruct > idMotiveViaGuid( const string &guid ); // returns id, motive, date and time of a ban by looking up the guid
        vector< idMotiveStruct > idMotiveViaIp( const string &ip );     // returns id, motive, date and time of a ban by looking up the ip
        vector< idMotiveStruct > idMotiveViaNick( const string &nick ); // returns id, motive, date and time of a ban by looking up the nick

        /* custom queries for analyzer */
        //"how many?" queries
        string autoBanned();                                        // returns how many users have been autobanned by the bot
        string banned();                                            // returns how many have been banned
        string ops();                                               // returns how many ops are registered to the bot

        string adminRegisteredNickViaGuid( const string &guid );    // returns the nick which with the admin is registered to the bot

        //"find" queries
        vector< idNickMotiveAuthorStruct > findAproxIdMotiveAuthorViaNickBanned( const string &nick );      // returns idNickMotiveAuthor struct after aprox find query ( BANNED TABLE )
        vector< idNickMotiveAuthorStruct > findPreciseIdMotiveAuthorViaNickBanned( const string &nick );    // returns idNickMotiveAuthor struct after precise find query ( BANNED TABLE )

        vector< idNickStruct > findAproxIdNickViaNickOp( const string &nick );                              //  returns idNick struct after aprox find query ( OPLIST TABLE )
        vector< idNickStruct > findPreciseIdNickViaNickOp( const string &nick );                            // returns idNick struct after precise find query ( OPLIST TABLE )

    private:
        bool connect();                                                     // connects to the sqlite3 database
        bool copyFile( const string &source, const string &destination );   // copy a file to another one
        void createDb();                                                    // creates database
        string errorCodeToString( int errorCode ) const;
        bool execQuery( const string &query );                              // executes query and returns status to tell if the operation went well or not
        string getAdminNick( const string &guid );                          // return's admin's nick from his guid
//         void loadAdminlist( vector<ConfigLoader::AdminList> admins );
//         void loadBanlist( vector<ConfigLoader::Banlist> banned );
        int resultQuery( const string &query );                             //if query fails -> -1 else return number of elements found

        sqlite3 *m_database;
        ConfigLoader::Options* m_options;                                   // where i get all my info from ( bot related )

        char *m_zErrMsg;
        char **m_result;
        int m_resultCode;                                                   // return code i get from query ( use to check query returns. SQLITE_OK ecc )
        int m_nrow, m_ncol;                                                 // number of rows and columns

        // data is stored here after query executions!!
        vector< string > m_vcolHead;                                        // MUST clear otherwise i keep old values as well
        vector< string > m_data;                                            // the info i need is stored here after a query
};

#endif //DB_H
