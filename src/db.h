#ifndef DB_H
#define DB_H

#include <iostream>
#include <fstream>
#include <string>
#include "sqlite3.h"

using namespace std;//std::string;

class db
{
    public:
        db();
        ~db();
        
        void create();
        void ban( string );
        bool checkAuthGuid( string );
        void checkBanGuid( string, string );
        void write();
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
        string aux;
};


#endif //DB_H