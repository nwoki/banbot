#include <iostream>
#include <string>
#include "db.h"
#include "connection.h"
#include "sqlite3.h"

using namespace std;

#define IP "81.174.67.195"  //server ip
#define PORT 27960  //server port
#define RCON "test" //server rcon password
#define LOG "botLog.log"  //where logfile is located

int main( int argc, char *argv[] ){ //pass arguments to specify server logfile and bot logfile
    
    if( argv[3] ){
        cout<<"[!] Wrong usage!\n"
            "./Banbot <server logfile> <bot logfile>\n";
    }
    else if( argv[1] && !argv[2] ){
        cout<<"[!] Setting up bot with:\n"
            "~ server logfile @ "<<argv[1]<<endl;
        //check existance of server logfile
    }
    else if( argv[1] && argv[2] ){
        cout<<"[!] Setting up bot with:\n"
            "~ server logfile @ "<<argv[1]<<endl;
        cout<<"~ bot logfile @ "<<argv[2]<<endl;
    }
    else{
        cout<<"[!] Using default settings from config.cfg..\n";
        //use config class to loag info
    }
}

    /*db d;
    string a, b;
    a.append( "123" );
    b.append("asder");
    d.checkBanGuid( a, b );*/
    
    /*connection serverCommand( IP, PORT, RCON );
    
    if( serverCommand.ban("ghj") )
        cout<<"[!] sucessfully banned player\n";
    else
        cout<<"[ ERR main ] couldn't ban player\n";

}*/