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
        
        string read();
        void write();
        string query( string );
        void close();
        
    private:
        sqlite3 *database;
        string queryStr;
};


#endif //DB_H