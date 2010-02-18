#include "db.h"

db::db()
{
    try{    //check per il file log
        ifstream IN( "lbotLog.log" );
        if( !IN ) throw( 0 );
        else IN.close();
    }
    catch( int x ){
        cout<<"[!] Logfile doesn't exist..\n";
        //create logfile
        
    }
    
    try{    //check della directory database
        struct stat st;
        
        cout<<"[*]checking for database directory..\n";
        if( stat( "database", &st ) == 0 )
            cout<<"[*]dir 'database/' found\n";
        
        else{
            cout<<"[!]couldn't find dir 'database'! Creating dir 'database'..\n";
            
            if( !system( "mkdir database" ))
                cout<< "[ OK ]created 'database' directory..\n";
            else throw 0;
        }
    }
    catch( int x ){
        cout<<"[ ERR ] couldn't create directory 'database'.Please check permissions!\n";
    }
    
    try{    //check per il database
        ifstream IN( "database/db.sqlite" );
        if( !IN ) throw ( 0 );
        else IN.close();
    }
    
    catch( int x ){
        cout<<"[!] database doesn't exist!\n";
        //create database
        if( sqlite3_open( "database/db.sqlite", &database ) != SQLITE_OK )
            cout<< "[ ERR ] " << sqlite3_errmsg( database ) << endl;
        else
            cout<< "[*] creating database 'db.sqlite' in 'database/'\n";
        createDb();
    }       
}

db::~db()
{
    //delete database;
}

void db::createDb()   //initial creation of database
{
    //create tables, oplist(nick, guid) and banned(guid)
    queryStr = "create table banned(guid text)";
    if( execQuery( queryStr ) )
        cout<< "[*]created banned table..\n";
    
    //oplist
    queryStr = "create table oplist(nick text, guid text)";
    if ( execQuery( queryStr ) )
        cout<< "[*]created oplist table..\n";

    //close database
    close();
}

void db::createLogFIle()
{

}

//checks oplist for ops
bool db::checkAuthGuid( string guid )
{
    aux.clear();
    aux.append( "select guid from oplist where guid='" );
    aux.append( guid );
    aux.append( "'" );
    
    queryStr = (char *)aux.c_str();
    
    return ( resultQuery( queryStr ) );
}

bool db::checkBanGuid( string banGuid )
{   
    aux.append( "select guid from banned where guid='" );
    aux.append( banGuid );
    aux.append( "'" );
    
    queryStr = (char *)aux.c_str();
    
    return ( resultQuery( queryStr ));
}

bool db::execQuery( const char *a )
{
    bool answer;
    cout<<"exec query-> "<<a<<endl;
    if( sqlite3_open( "database/db.sqlite", &database ) != SQLITE_OK ){
        printf( "[ERR] can't execute query ** %s **\nERROR->%s \n", a, errorMsg );
        sqlite3_free( errorMsg );
        
        answer = false;   //went bad
    }
    else{
        if ( sqlite3_exec( database, a, NULL, NULL, &errorMsg ) != SQLITE_OK){
            cout<<"[ERR] "<<errorMsg<<endl;
            sqlite3_free( errorMsg );
            
            answer = false;
        }
        else  answer = true;
    }
    //close db
    close();
    return answer;
}

bool db::resultQuery( const char *a )
{    
    bool answer;
    
    if( sqlite3_open( "database/db.sqlite", &database ) != SQLITE_OK ){
        printf( "[ERR] can't execute query ** %s **\nERROR->%s \n", a, errorMsg );
        sqlite3_free( errorMsg );
        
        answer = false;   //went bad
    }
    else{
        cout<<"result query -> "<<a<<endl;
        
        if( sqlite3_prepare_v2( database, a, -1, stmt, NULL ) == SQLITE_OK ){ //proceed
            sqlite3_step( *stmt );
            result = sqlite3_column_text( *stmt, 0 );
            sqlite3_finalize( *stmt );
            
            if( !result ){
                cout<<"empty or can't exec query\n";
                answer = false;
            }
            else{
                cout<<"got answer or execed the query\n";
                answer = true;
            }
        }
    }
    //close db
    close();
    return answer;
}

void db::ban( string guid )
{
    //add 
    if( !aux.empty() )
        aux.clear();
    
    aux.append( "insert into banned values('" );
    aux.append( guid );
    aux.append( "')" );
    
    queryStr = (char *)aux.c_str();
    
    bool check = execQuery( queryStr );
    
    if( check ) cout<<"ban applied\n";
    else cout<<"didn't apply ban\n";
    
}

//entra persona con guid = 1234;
//controllo guid e banno

void db::close()
{
    sqlite3_close( database );
}
