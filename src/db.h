#ifndef DB_H
#define DB_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include "sqlite3.h"

using namespace std;//std::string;

class db
{
    public:
        db();
        ~db();
        
        void createDb();
        void createLogFIle();
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
        char *queryStr;
        char *errorMsg;
        const unsigned char *result;
        string aux, logPathS;
};


#endif //DB_H