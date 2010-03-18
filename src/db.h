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
#include "ConfigLoader.h"
#include "sqlite3/sqlite3.h"
//#include "logger.h"
class Logger;


#define DATABASE "database/Db.sqlite"

using namespace std;//std::string;

class Db
{
    public:
        //Db( Logger *, vector<ConfigLoader::Option> ); //passo array di guid secondo parametro
        Db( vector<ConfigLoader::Option>, vector<ConfigLoader::Banlist>, Logger * );
        ~Db();

        bool ban( const string &guid );
        bool checkAuthGuid( const string &guid );
        bool checkBanGuid( const string &guid );    //passa ( guid giocatore)
        bool checkDirAndFile( const string &guid ); //passa url file compreso
        void dumpBanned();

    private:
        sqlite3 *database;
        void createDb();
        void setupAdmins( vector<ConfigLoader::Option> );
        //void loadAdminlist( vector<ConfigLoader::Option> ;  ///NEW
        void loadBanlist( vector<ConfigLoader::Banlist> );
        int resultQuery( const string & ); //se fallisce la query, -1, altrimenti il numero degli elementi restituiti
        std::vector<std::string> extractData( const string &query );   //returns vector with results as string
        void close();
        bool connect();
        Logger *logger;

	class SQLITE3
	{
	  private:
	    sqlite3 *db;
	    char *zErrMsg;
	    char **result;
	    int rc;
	    int nrow, ncol;
	    int db_open;

	  public:

	    std::vector<std::string> vcol_head;
	    std::vector<std::string> vdata;

	    SQLITE3 ( const std::string &database ):
            zErrMsg( 0 ),
            rc( 0 ),
            db_open( 0 )
	    {
	      rc = sqlite3_open(database.c_str(), &db);

          if( rc ){
              cout<<"Can't open database\n";
              sqlite3_close( db );
	      }
	      db_open = 1;
	    }

	    int exe( const std::string &s_exe)
	    {
	        rc = sqlite3_get_table(
                db,              /* An open database */
                s_exe.c_str(),       /* SQL to be executed */
                &result,       /* Result written to a char *[]  that this points to */
                &nrow,             /* Number of result rows written here */
                &ncol,          /* Number of result columns written here */
                &zErrMsg          /* Error msg written here */
                );

	        if( vcol_head.size() < 0 )
                vcol_head.clear();

            if( vdata.size() < 0 )
                vdata.clear();

  	        if( rc == SQLITE_OK ){
                for( int i = 0; i < ncol; ++i )
                    vcol_head.push_back( result[i] );   /* First row heading */
                for( int i = 0; i < ncol*nrow; ++i )
                    vdata.push_back( result[ncol+i] );
	        }
	        sqlite3_free_table( result );
            return rc;
	    }


	    ~SQLITE3()
	    {
	      sqlite3_close(db);
	    }
	};

};


#endif //DB_H