#include <iostream>
#include <string>
#include "db.h"
#include "connection.h"
#include "sqlite3.h"

using namespace std;

#define IP "localhost"  //server ip
#define PORT 27960  //server port
#define RCON "asd" //server rcon password
#define LOG "botLog.log"  //where logfile is located

int main( int argc, char *argv[] ){ //pass arguments to specify server logfile and bot logfile

    /*Db d;
    string a, b;
    a.append( "123" );
    b.append("asder");
    d.checkBanGuid( b );*/

    Connection serverCommand( IP, PORT, RCON );

    if( !serverCommand.kick("asdasd") )
        cout<<"[!] sucessfully banned player\n";
    else
        cout<<"[ ERR main ] couldn't ban player\n";

}