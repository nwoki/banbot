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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/


#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <vector>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ConfigLoader.h"
#include <stdlib.h>

#define _VERSION "1.2b1"

#define SOCKET_PAUSE 600000  //per permettere al socket di funzionare bene, inserisco una pausa, in microsecondi (1'000 = 1 ms, 1'000'000= 1 s)

class Connection
{
    public:
        Connection(ConfigLoader::Options* opzioni);
        ~Connection();

        void kick( std::string number, int server );  //number of the player to kick on @server.
        void say( std::string frase, int server );    //phrase to print publically on @server.
        void bigtext( std::string frase, int server );    //phrase to print publically (big) on @server.
        void tell( std::string frase, std::string player, int server ); //@frase is the private message to send to @player.
        void reload( int server );            //does a reload of the current map. If @server is -1, it does a reload on all servers.
        void mute( std::string number, int server ); //mute/unmute the player @number.
        void muteAll( std::string admin, int server ); //mute/unmute all players except @admin (number).
        void veto( int server );                  //does a veto.
        void slap( std::string number, int server ); //slaps the player @number.
        void nuke( std::string number, int server ); //nukes the player @number.
        void force( std::string number, std::string where, int server ); //move the player @number on the team @where
        void map( std::string name, int server ); //change the map
        void nextmap( std::string name, int server ); //change the nextmap
        void changePassword( std::string pass, int server ); //change the password of the server
        void exec( std::string file, int server ); //load a config file
        void restart( int server ); //restart the current match
        void cycle( int server ); //restart the current match
        void gravity ( std::string amount, int server);  //changes the gravity on the server.
        void sendInfo ();
        
        std::string status ( int server ); //request the status at the server.
        
    private:
        //class packets, used to receive the rcon message and rebuild it.
        class Packets {
            private:
                std::vector<std::string> p;
                std::string clean (std::string &message){
                    int i = message.find_last_of('\n');
                    return message.substr(i+1);
                }
            public:
                Packets(){}
                void addPacket(std::string packet){p.push_back(packet);}
                std::string rebuild(){
                    if (p.size() == 0) return "";
                    if (p.size() == 1) return clean( p[0] );
                    //check if the last packet has arrived in the wrong order.
                    for (unsigned int i=0; i < p.size()-1; i++){
                        if ( p[i].find("\n\n") < p[i].size() ) {
                            p.push_back(p[i]);
                            p.erase(p.begin()+i);
                        }
                    }
                    //rebuild the message
                    std::string result;
                    for (unsigned int i=0; i<p.size(); i++)
                        result.append(clean(p[i]));
                    return result;
                }
        };
        
        
        sockaddr_in serverAdd;// clientAdd;
        int socketID;
        int wait_packets(long sec,long usec); //wait for the response
        void receive(Packets *);

        void prepareConnection( int );
        std::vector<char> makeCmd( std::string );

        ConfigLoader::Options* m_options;
};




#endif  //CONNECTION_H
