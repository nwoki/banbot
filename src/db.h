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

using namespace std;//std::string;

class Db
{
    public:
        //Db( Logger *, vector<ConfigLoader::Option> ); //passo array di guid secondo parametro
	Db(vector<ConfigLoader::Option> );
        ~Db();

        void createDb();
        bool connect();
        void ban( string );
        bool checkAuthGuid( string );
        bool checkBanGuid( string );    //passa ( guid giocatore)

        bool execQuery( const char * );
        void setupAdmins();
        bool resultQuery( const char * );
        void close();

    private:
        int dbStatus;
        sqlite3 *database;
        sqlite3_stmt **stmt;
        char *queryStr, *errorMsg;
        const unsigned char *result;
        string aux, botLog, serverLog;
        //Logger *logger;
        vector<ConfigLoader::Option> opzioni;
};


#endif //DB_H