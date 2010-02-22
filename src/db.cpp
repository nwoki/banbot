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
Db::Db(vector<ConfigLoader::Option> conf )
    //logger( log )
{
    //logger=log;
    //comincio il logging
    //logger->open();
    //check della directory database
    struct stat st;

    cout<<"[*]Starting up...\n";
    cout<<"[-]checking for database directory..\n";
    if( stat( "database", &st ) == 0 )
        cout<<"  [*]dir 'database/' found\n";
    else{
        "  [!]couldn't find dir 'database'! Creating dir 'database'..\n";

        if( !system( "mkdir database" ))
            cout<<"  [OK]created 'database' directory..\n";
        else
            cout<<"[EPIC FAIL] couldn't create directory 'database'.Please check permissions!\n";
    }

    //check per il database
    ifstream IN( DATABASE );
    if( !IN )
    {
         cout<<"  [!] database doesn't exist!\n";
         //create database
         cout<<"    [*] creating database 'db.sqlite' in 'database/'\n";
	 createDb();
    }
    IN.close();
    //azzero la tabella degli admins
    setupAdmins(conf);
}

Db::~Db()
{
    //logger->close();
    //delete logger;
}

void Db::setupAdmins(vector<ConfigLoader::Option> opzioni)
{
    cout<<"  [-] setting up admin guid's.. \n";

    string clearQuery("delete from oplist;");

    if( resultQuery( clearQuery)==0 )
        cout<<"    [*]cleaned admin table..\n";
    else cout<<"    [EPIC FAIL] Db::setupAdmins can't execute query!\n";

    cout<<"    [-]trying to repopulate database\n";

    for( int i = 0; i < opzioni.size(); i++){
        if( opzioni[i].name.compare("GUID")==0 ){

            string aux;
            aux.append("insert into oplist values('");
            aux.append(opzioni[i].value);
            aux.append("');");;

            if ( resultQuery( aux )==0 )
                cout<<"      [+]added admin.. \n";
            else cout<<"      [FAIL] can't add admin to database!\n";
        }
    }
}


bool Db::connect()
{
    if(sqlite3_open( DATABASE, &database )){
        cout<<"[FAILss] " << sqlite3_errmsg( database );    //TODO
        close();
        return false;
    }
    else return true;
}

void Db::createDb()   //initial creation of database
{
    //create tables, oplist(nick, guid) and banned(guid)
    string queryStr("create table banned(guid text)");
    if( resultQuery( queryStr )==0 )
        cout<<"    [*]created banned table..\n";
    else cout<<"    [FAIL]error creating banned table\n";

    //oplist
    queryStr.clear();
    queryStr.append("create table oplist(guid text)");
    if ( resultQuery( queryStr )==0 )
        cout<<"    [*]created oplist table..\n";
    else cout<<"    [FAIL]error creating oplist table\n";
}

//checks oplist for ops
bool Db::checkAuthGuid( string guid )
{
    string aux( "select guid from oplist where guid='" );
    aux.append( guid );
    aux.append( "'" );

    return (resultQuery( aux )>0);
}

bool Db::checkBanGuid( string banGuid )
{
    string aux( "select guid from banned where guid='" );
    aux.append( banGuid );
    aux.append( "'" );

    cout<<"[-] Test sul ban "<<banGuid<<"\n";
    return ( resultQuery( aux )>0);
}

int Db::resultQuery( string a )
{
    int answer=0;
    SQLITE3 *query=new SQLITE3(DATABASE);
    if (query->exe(a)) answer=-1;
    else answer=query->vdata.size();
    delete (query);
    return answer;
}

void Db::ban( string guid )
{
    //add
    string aux( "insert into banned values('" );
    aux.append( guid );
    aux.append( "');" );

    if( resultQuery( aux )==0 )
        cout<<"[OK] ban applied on "<<guid<<"\n";
    else cout<<"[FAIL] ban not applied on "<<guid<<"\n";
}

void Db::close()
{
    sqlite3_close( database );
}
