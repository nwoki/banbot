#include <iostream>
#include <string>
#include "db.h"
#include "connection.h"
#include "sqlite3.h"

using namespace std;

#define IP "81.174.67.195"  //server ip
#define PORT 27960  //server port
#define RCON "test" //server rcon password
#define LOG ""  //where logfile is located

int main(){


    /*db d;
    string a, b;
    a.append( "123" );
    b.append("asder");
    d.checkBanGuid( a, b );*/
    
    connection serverCommand( IP, PORT, RCON );
    
    if( serverCommand.ban("ghj") )
        cout<<"[!] sucessfully banned player\n";
    else
        cout<<"[ ERR main ] couldn't ban player\n";

}