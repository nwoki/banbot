/*              db.
 *
 *  Author:     [2s2h]n3m3s1s
 *
 *  Description:    Classe che gestisce il database sqlite3 e restituisce i
 *                  risultati delle query alla classe analyzer che poi decide
 *                  come proseguire
*/


#include "db.h"

//Db::Db( Logger * log, vector<ConfigLoader::Option> conf ):
Db::Db(vector<ConfigLoader::Option> conf ):
    //logger( log ),
    opzioni( conf )
{
    //logger=log;
    //comincio il logging
    //logger->open();
    //check della directory database
    struct stat st;

    cout<<"[-]checking for database directory..\n";
    if( stat( "database", &st ) == 0 )
        cout<<"[*]dir 'database/' found";
    else{
        "[!]couldn't find dir 'database'! Creating dir 'database'..\n";

        if( !system( "mkdir database" ))
            cout<<"[OK]created 'database' directory..\n";
        else
            cout<<"[ERR] couldn't create directory 'database'.Please check permissions!\n";
    }

    //check per il database
    ifstream IN( "database/db.sqlite" );
    if( IN ) IN.close();
    else{
        cout<<"[!] database doesn't exist!\n";
        if( connect() ){
            //create database
            createDb();
            cout<<"[*] creating database 'db.sqlite' in 'database/'\n";
        }
    }
    
    //azzero la tabella degli admins
    setupAdmins();
}

Db::~Db()
{
    //logger->close();
    //delete logger;
}

bool Db::connect()
{
    if( sqlite3_open( "database/db.sqlite", &database ) != SQLITE_OK ){
        cout<<"[ERR] " << sqlite3_errmsg( database );    //TODO
        sqlite3_free( errorMsg );
        return false;
    }
    else return true;
}

void Db::createDb()   //initial creation of database
{
    //create tables, oplist(nick, guid) and banned(guid)
    queryStr = "create table banned(guid text)";
    if( execQuery( queryStr ) )
        cout<<"[*]created banned table..\n";

    //oplist
    queryStr = "create table oplist(guid text)";
    if ( execQuery( queryStr ) )
        cout<<"[*]created oplist table..\n";

    close();    //close database
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
    aux.clear();
    aux.append( "select guid from banned where guid='" );
    aux.append( banGuid );
    aux.append( "'" );

    queryStr = (char *)aux.c_str();

    cout<<"[*] chiamata a resultquery\n";
    return ( resultQuery( queryStr ));
}

bool Db::execQuery( const char *a )
{
    bool answer;
    cout<<"exec query-> "<<a<<"\n";

    if( !connect() )    //went bad
        answer = false;
    else{
        if ( sqlite3_exec( database, a, NULL, NULL, &errorMsg ) != SQLITE_OK){
            cout<<"[ERR] "<<errorMsg<<"\n";
            sqlite3_free( errorMsg );

            answer = false;
        }
        else  answer = true;
    }
    close();    //close db
    return answer;
}

bool Db::resultQuery( const char *a )
{
    bool answer=false;
    cout<<"Diocane??\n";

    if( !connect() )
        answer = false;   //went bad
    else{
        cout<<"result query -> "<<a<<"\n";

        if( sqlite3_prepare_v2( database, a, -1, stmt, NULL ) == SQLITE_OK )
	{
	  cout<<"[-] entrato nell'if \n";
	  //proceed
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
    close();    //close db
    return answer;
}

void Db::ban( string guid )
{
    //add
    if( !aux.empty() )
        aux.clear();

    aux.append( "insert into banned values('" );
    aux.append( guid );
    aux.append( "')" );

    queryStr = (char *)aux.c_str();

    if( execQuery( queryStr ) )
        cout<<"ban applied\n";
    else cout<<"didn't apply ban\n";
}

void Db::setupAdmins()
{
    cout<<"[-] setting up admin guid's.. \n";

    char *clearQuery = "delete from oplist;";
    connect();

    if( execQuery( clearQuery ) )
        cout<<"[*]cleaned admin table..\n";
    else cout<<"[ ERR Db::setupAdmins ] can't execute query!\n";

    cout<<"[-]trying to repopulate database\n";

    for( int i = 0; i < opzioni.size(); i++){
        if( opzioni[i].name.compare("GUID")==0 ){

            string aux;
            aux.append("insert into oplist values('");
            aux.append(opzioni[i].value);
            aux.append("');");

            connect();
            queryStr = (char*)aux.c_str();

            if ( execQuery( queryStr ) )
                cout<<"[*]added admin.. \n";
            else cout<<"[ ERR ] can't add admin to database!\n";
        }
    }
}

void Db::close()
{
    sqlite3_close( database );
}
