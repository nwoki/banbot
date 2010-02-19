#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <netdb.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
//#include <sys/types.h>

using namespace std;

class Connection
{
    public:
        Connection( string, int, string );
        ~Connection();
        
        //int ban( string );
        void kick( string );
        
    private:
        int create();
        
        //needed for socket
        struct sockaddr_in server;   //server a cui mi connetto
        struct hostent *hp;
        char *buff;
        int length; //message length
        
        
        const char *ip;
        const int port;   
        int conn;
        string rconPass;
};




#endif  //CONNECTION_H