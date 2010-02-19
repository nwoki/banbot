#include "db.h"

Db::Db()
{
  //check per il file log
  ifstream IN( "lbotLog.log" );
  if( IN.is_open() ) IN.close();
  else 
  {
    cout<<"[!] Logfile doesn't exist..\n";
    //create logfile
  }
    
  //check della directory database
  struct stat st;
        
  cout<<"[*]checking for database directory..\n";
  if( stat( "database", &st ) == 0 )
      cout<<"[*]dir 'database/' found\n";
  
  else
  {
    cout<<"[!]couldn't find dir 'database'! Creating dir 'database'..\n";
    
    if( !system( "mkdir database" ))
	cout<< "[ OK ]created 'database' directory..\n";
    else 
      cout<<"[ ERR ] couldn't create directory 'database'.Please check permissions!\n";
  }
    
  //check per il database
  ifstream IN( "database/db.sqlite" );
  if( IN ) IN.close();
  else 
  {
    cout<<"[!] database doesn't exist!\n";
    //create database
    if( sqlite3_open( "database/db.sqlite", &database ) != SQLITE_OK )
	cout<< "[ ERR ] " << sqlite3_errmsg( database ) << endl;
    else
	cout<< "[*] creating database 'db.sqlite' in 'database/'\n";
    createDb();
  }       
}

Db::~Db()
{
    //delete database;
}

void Db::createDb()   //initial creation of database
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

void Db::createLogFIle()
{

}

//checks oplist for ops
bool Db::checkAuthGuid( string guid )
{
    aux.clear();
    aux.append( "select guid from oplist where guid='" );
    aux.append( guid );
    aux.append( "'" );
    
    queryStr = (char *)aux.c_str();
    
    return ( resultQuery( queryStr ) );
}

bool Db::checkBanGuid( string banGuid )
{   
    aux.append( "select guid from banned where guid='" );
    aux.append( banGuid );
    aux.append( "'" );
    
    queryStr = (char *)aux.c_str();
    
    return ( resultQuery( queryStr ));
}

bool Db::execQuery( const char *a )
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

bool Db::resultQuery( const char *a )
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

void Db::ban( string guid )
{
    //add 
    //if( !aux.empty() )
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

void Db::close()
{
    sqlite3_close( database );
}
