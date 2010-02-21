#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <vector>
#include <netdb.h>
#include <string>
#include <string.h>
//#include <strings.h>
#include <sys/socket.h>
//#include <sys/types.h>

using namespace std;

class Connection
{
    public:
        Connection( string, int, string );
        ~Connection();

        //void ban( string );  //faccio tornare la risposta del server??(string or *char)
        void kick( string );
        //void asd();

    private:
        sockaddr_in serverAdd, clientAdd;
        int socketID;
        struct hostent *hp;
        int recvSize; //message length in risposta

	void prepareConnection();
	vector<char> makeCmd( string);

        char *ip;
        const int port;
        int conn;
        string rconPass;
};




#endif  //CONNECTION_H
