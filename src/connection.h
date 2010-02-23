/*
    connection.h is part of BanBot.

    BanBot is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BanBot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BanBot (look at GPL_License.txt).
    If not, see <http://www.gnu.org/licenses/>.

    Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/


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
	void say( string );
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
