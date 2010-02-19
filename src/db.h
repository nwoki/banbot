#ifndef DB_H
#define DB_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include "sqlite3.h"

using namespace std;//std::string;

class Db
{
    public:
        Db();
        ~Db();

        void createDb();
        bool connect();
        void ban( string );
        bool checkAuthGuid( string );
        bool checkBanGuid( string );    //passa ( guid giocatore)

        bool execQuery( const char * );
        bool resultQuery( const char * );
        void close();

    private:
        int dbStatus;
        sqlite3 *database;
        sqlite3_stmt **stmt;
        char *queryStr, *errorMsg;
        const unsigned char *result;
        string aux, botLog, serverLog;
};


#endif //DB_H