#include "db.h"

db::db()
{
    try{
        struct stat st;
        
        cout<<"[*]checking for database directory..\n";
        if( stat( "database", &st ) == 0 )
            cout<<"[*]dir 'database/' found\n";
        
        else{
            cout<<"[!]couldn't find dir 'database'! Creating it..\n";
            
            if( !system( "mkdir database" ))
                cout<< "[*]created 'database' directory..\n";
            else throw 0;
        }
    }
    catch( int x ){
        cout<<"----\n";
    }
    
    try{
        ifstream IN( "database/db.sqlite" );
        if( !IN ) throw ( 0 );
        else IN.close();
    }
    
    catch( int x ){
        cout<<"[!] database doesn't exist!\n";
        //create database
        if( sqlite3_open( "database/db.sqlite", &database ) != SQLITE_OK )
            cout<< "[ERR] " << sqlite3_errmsg( database ) << endl;
        else
            cout<< "[*] creating database 'db.sqlite' in 'database/'\n ";
        create();
    }       
}

db::~db()
{
    //delete database;
}

void db::create()   //initial creation of database
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

//checks oplist for ops
bool db::checkAuthGuid( string guid )
{
    aux.clear();
    aux.append( "select guid from oplist where guid='" );
    aux.append( guid );
    aux.append( "'" );
    
    queryStr = (char *)aux.c_str();
    
    bool check = resultQuery( queryStr );
    return check;
}

void db::checkBanGuid( string authGuid, string banGuid )
{   
    if( checkAuthGuid( authGuid ) ){
        //clear aux if already used
        if( !aux.empty() )
            aux.clear();
        
        aux.append( "select guid from banned where guid='" );
        aux.append( banGuid );
        aux.append( "'" );
        
        queryStr = (char *)aux.c_str();
    
        bool check = resultQuery( queryStr );
        
        if( check ){    //esiste, vai con kick!!
            cout<<"found ban\n";
            //kick through connect
        }
        else{
            cout<<"no ban\nadding to db\n";
            //add ban 
            ban( banGuid );
            //then kick
        }
    }
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
            cout<<"fuck\n";
        }
        else{
            answer = true;
            cout<<"yeah\n";
        }
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
