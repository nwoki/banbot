#include "db.h"

db::db()
{
    //check if database already exists. In case create it
    //database = new sqlite3();    
    try{
        ifstream IN("database/db.sqlite");
        if( !IN ) throw (0);
        else IN.close();
    }
    
    catch(int x){
        cout<<"[!] database doesn't exist!\n";
        //create database
        if( sqlite3_open( "database/db.sqlite", &database ) != SQLITE_OK )
            cout<<"[ERR] can't create db %s \n",sqlite3_errmsg( database );
        else
            cout<<"[*] creating database 'db.sqlite' in 'database/'\n";
    }    
}

db::~db()
{
    //delete database;
}

string db::read()
{

}

void db::write()
{

}

void db::close()
{
    sqlite3_close( database );
}
