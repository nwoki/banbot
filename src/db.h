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
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "ConfigLoader.h"
#include "sqlite3/sqlite3.h"

class Logger;

#define DATABASE "database/Db.sqlite"
#define DATABASE_DIR "database"

using namespace std;

class Db
{
    public:
        Db( vector< ConfigLoader::Option >, vector< ConfigLoader::Banlist >, vector< ConfigLoader::Option >, Logger * );
        ~Db();

        //database connection
        bool openDatabase();    //opens a connection with the sqlite3 database
        void closeDatabase();   //closes a connection with the sqlit3 database
        
        //checks
        bool checkAuthGuid( const string &guid );   //controlla l'esistenza del guid passato (OPLIST)
        bool checkBanGuid( const string &guid );    //passa ( guid giocatore)
        bool checkBanNick( const string &nick );    //controlla l'esistenza del nick nella tabella dei bannati
        //bool checkDirAndFile( const string &guid ); //passa url file compreso

        //dumps
        void dumpAdminsToFile();
        void dumpBannedToFile();
        void dumpDatabase();

        //banned table
        bool ban( const string &nick, const string &ip, const string &date, const string &time, const string &guid, const string &motive, const string &adminGuid );
        string insertNewBanned( const string &nick, const string &ip,  const string &date, const string &time, const string &motive, string adminNick );
        bool modifyBanned( const string &nick, const string &ip,  const string &date, const string &time, const string &motive, const string &id );
        bool deleteBanned( const string &id );

        //guid table
        string insertNewGuid( const string &guid, const string &banId );
        bool modifyGuid( const string &guid, const string &banId, const string &id );
        bool deleteGuid( const string &id );

        //oplist table
        bool addOp( const string &nick, const string &guid );
        bool modifyOp( const string &nick, const string &guid, const string &id );
        bool deleteOp( const string &id );

        vector< string > extractData( const string &query );   //ritorna un vettore con i/il risultato della query

    private:
        bool connect();
        void createDb();
        bool execQuery( const string &query );    //esegue e ritorna status per indicare se l'operazione è andato a buon fine
        string getAdminNick( const string &guid );  //return's admin's nick from his guid
        string intToString( int number );
        void loadAdminlist( vector<ConfigLoader::Option> admins );
        void loadBanlist( vector<ConfigLoader::Banlist> banned );
        int resultQuery( const string &query );    //se fallisce la query, -1, altrimenti restituisce il numero degli elementi trovati


        sqlite3 *m_database;
        Logger *m_logger;

        //testing
        char *m_zErrMsg;
        char **m_result;
        int m_resultCode; //return code i get from query ( use to check query returns. SQLITE_OK ecc )
        int m_nrow, m_ncol; //number of rows and columns

        //data is stored here after query executions
        vector< string > m_vcolHead; /*MUST clear otherwise i keep old values as well*/
        vector< string > m_data; /*MUST clear otherwise i keep old values as well*/
};

#endif //DB_H
