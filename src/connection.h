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
#include <sys/socket.h>
#include "ConfigLoader.h"
#include <stdlib.h>

#define SOCKET_PAUSE 600000  //per permettere al socket di funzionare bene, inserisco una pausa, in microsecondi (1'000 = 1 ms, 1'000'000= 1 s)

using namespace std;

class Connection
{
    public:
        Connection(ConfigLoader::Options* opzioni);
        ~Connection();

        void kick( string number, int server );  //number of the player to kick on @server.
        void say( string frase, int server );    //phrase to print publically on @server.
        void bigtext( string frase, int server );    //phrase to print publically (big) on @server.
        void tell( string frase, string player, int server ); //@frase is the private message to send to @player.
        void reload( int server );            //does a reload of the current map. If @server is -1, it does a reload on all servers.
        void mute( string number, int server ); //mute/unmute the player @number.
        void muteAll( string admin, int server ); //mute/unmute all players except @admin (number).
        void veto( int server );                  //does a veto.
        void slap( string number, int server ); //slaps the player @number.
        void nuke( string number, int server ); //nukes the player @number.
        void force( string number, string where, int server ); //move the player @number on the team @where
        void map( string name, int server ); //change the map
        void nextmap( string name, int server ); //change the nextmap
        void changePassword( string pass, int server ); //change the password of the server
        void exec( string file, int server ); //load a config file

    private:
        sockaddr_in serverAdd;// clientAdd;
        int socketID;
        struct hostent *hp;
        int recvSize; //message length in risposta

        void prepareConnection( int );
        vector<char> makeCmd( string );

        ConfigLoader::Options* m_options;

        /*vector<char *>ip;
        vector<int> port;
        vector<string>rconPass;*/
};




#endif  //CONNECTION_H
