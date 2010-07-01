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

#define SOCKET_PAUSE 1  //per permettere al socket di funzionare bene, inserisco una pausa tra say e kick
#define ROW 100         //numero massimo di caratteri per riga

using namespace std;

class Connection
{
    public:
        Connection(ConfigLoader::Options* opzioni);
        ~Connection();

        void kick( string number );  //prende il numero del giocatore da buttare fuori e il numero del server.
        void say( string frase );    //prende la frase da scrivere pubblicamente ed il numero del server.
        void tell( string frase, string player ); //invia il messaggio privato "frase" al giocatore del numero contenuto in "player", nel server "server".
        void reload(bool all=false);            //fa un reload della mappa. Se viene passato true, lo fa su tutte le mappe.
        void mute( string number ); //muta/smuta il player numero @number sul server @server
        void veto();                  //comando di veto sul server @server
        void slap( string number ); //slappa il giocatore @number del server @server
        void nuke( string number ); //nuke al giocatore @number del server @server
        void force( string number, string where ); //forza il player numero @number sul server @server nel team @where

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
