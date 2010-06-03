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
        bool openDatabase();
        void closeDatabase();
        //checks
        bool checkAuthGuid( const string &guid );   //controlla l'esistenza del guid passato (OPLIST)
        bool checkBanGuid( const string &guid );    //passa ( guid giocatore)
        bool checkBanNick( const string &nick );    //controlla l'esistenza del nick nella tabella dei bannati
        //bool checkDirAndFile( const string &guid ); //passa url file compreso

        //dumps
        void dumpAdminsToFile();
        void dumpBannedToFile();
        void dumpDatabase();    //TODO not yet implemented

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
        //void close();
        //void clearAfterOperation();
        bool connect();
        void createDb();
        bool execQuery( const string &query );    //esegue e ritorna status per indicare se l'operazione è andato a buon fine
        string getAdminNick( const string &guid );  //return's admin's nick from his guid
        string intToString( int number );
        void loadAdminlist( vector<ConfigLoader::Option> admins );
        void loadBanlist( vector<ConfigLoader::Banlist> banned );
        int resultQuery( const string &query );    //se fallisce la query, -1, altrimenti restituisce il numero degli elementi trovati


        sqlite3 *database;
        Logger *logger;

        //testing
        char *zErrMsg;
        char **result;
        int rc; //return int i get from query ( use to check query returns. SQLITE_OK ecc )
        int nrow, ncol; //number of rows and columns

        //data is stored here after query executions
        vector< string > vcol_head; /*MUST clear otherwise i keep old values as well*/
        vector< string > vdata; /*MUST clear otherwise i keep old values as well*/
};

/*

	class SQLITE3
	{
	  private:
	    sqlite3 *db;
	    char *zErrMsg;
	    char **result;
	    int rc;
	    int nrow, ncol;
	    //int db_open;

	  public:

	    vector< string > vcol_head;
	    vector< string > vdata;

	    SQLITE3 ( const string &database )
            : zErrMsg( 0 )
            , rc( 0 )
            //db_open( 0 )
	    {
	      rc = sqlite3_open(database.c_str(), &db);

          if( rc ){
              cout<<"\e[1;31m[EPIC FAIL] Can't open database: " << zErrMsg << "\e[0m \n";
              sqlite3_close( db );
	      }
	      //db_open = 1;
          //debug msg
          #ifdef DEBUG
              cout<< "\e[1;35mOPENED DB\e[0m \n";
          #endif
	    }

	    int exe( const string &s_exe)
	    {
	        rc = sqlite3_get_table(
                db,              /* An open database */
                //s_exe.c_str(),       /* SQL to be executed */
                //&result,       /* Result written to a char *[]  that this points to */
                //&nrow,             /* Number of result rows written here */
                //&ncol,          /* Number of result columns written here */
                //&zErrMsg          /* Error msg written here */
                //);
/*
	        if( vcol_head.size() < 0 )
                vcol_head.clear();

            if( vdata.size() < 0 )
                vdata.clear();

  	        if( rc == SQLITE_OK ){
                for( int i = 0; i < ncol; ++i )
                    vcol_head.push_back( result[i] );   /* First row heading */
                /*for( int i = 0; i < ncol*nrow; ++i )
                    vdata.push_back( result[ncol+i] );
	        }
	        else{
                cout <<"\e[1;31m[EPIC FAIL] SQLITE3::exe() " << zErrMsg << "\e[0m \n";// sqlite3_errmsg( db );
                //come faccio a loggare l'errore che ricevo qui??? TODO
                //*logger<< "[EPIC FAIL] SQLITE3::exe() " << zErrMsg << "\n";
            }
	        sqlite3_free_table( result );
            return rc;
	    }


	    ~SQLITE3()
	    {
	      sqlite3_close( db );
          //debug msg
          #ifdef DEBUG_MODE
              cout<< "\e[1;36mCLOSING DB\e[0m \n";
          #endif
	    }
	};

};*/



#endif //DB_H
