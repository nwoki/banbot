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

#define DATABASE "database/db.sqlite"
using namespace std;//std::string;

class Db
{
    public:
        //Db( Logger *, vector<ConfigLoader::Option> ); //passo array di guid secondo parametro
	Db(vector<ConfigLoader::Option> );
        ~Db();
	
        void ban( string );
        bool checkAuthGuid( string );
        bool checkBanGuid( string );    //passa ( guid giocatore)

    private:
        sqlite3 *database;
	void createDb();
	void setupAdmins(vector<ConfigLoader::Option>);
        int resultQuery( string ); //se fallisce la query, -1, altrimenti il numero degli elementi restituiti
        void close();
	bool connect();
	
	class SQLITE3 
	{
	  private:
	    sqlite3 *db;
	    char *zErrMsg;
	    char **result;
	    int rc;
	    int nrow,ncol;
	    int db_open;

	  public:

	    std::vector<std::string> vcol_head;
	    std::vector<std::string> vdata;

	    SQLITE3 (std::string tablename): zErrMsg(0), rc(0),db_open(0) 
	    {
	      rc = sqlite3_open(tablename.c_str(), &db);
	      if( rc ){
		cout<<"Can't open database\n";
		sqlite3_close(db);
	      }
	      db_open=1;
	    }
	
	    int exe(std::string s_exe) 
	    {
	      rc = sqlite3_get_table(
			      db,              /* An open database */
			      s_exe.c_str(),       /* SQL to be executed */
			      &result,       /* Result written to a char *[]  that this points to */
			      &nrow,             /* Number of result rows written here */
			      &ncol,          /* Number of result columns written here */
			      &zErrMsg          /* Error msg written here */
			      );

	      if(vcol_head.size()<0) { vcol_head.clear();  }
	      if(vdata.size()<0)     { vdata.clear(); }

	      if( rc == SQLITE_OK ){
		for(int i=0; i < ncol; ++i)
		vcol_head.push_back(result[i]);   /* First row heading */
		for(int i=0; i < ncol*nrow; ++i)
		vdata.push_back(result[ncol+i]);
	      }
	      sqlite3_free_table(result);
		return rc;
	    }

	    ~SQLITE3()
	    {
	      sqlite3_close(db);
	    }
	};

};


#endif //DB_H