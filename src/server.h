/*
    server.h is part of BanBot.

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

#ifndef _server_h_
#define _server_h_

#include <sys/stat.h>
#include <string>
#include <fstream>
#include <vector>

class Server
{
    public:
        Server();
        ~Server();

        class Player
        {
            public:
                std::string GUID;
                std::string number;
                std::string nick;
                std::string ip;
                Player() : GUID(""), number(""), nick(""), ip("") {};
                Player( std::string g, std::string nu, std::string ni, std::string i ) : GUID(g), number(nu), nick(ni), ip(i) {};
                Player* clone() { return new Player( GUID, number, nick, ip ); };
        };
        
        //getters
        std::string getName();
        std::string getBackupDir();
        std::string getBotLog();
        std::string getConfigFile();
        std::string getDbFolder();
        struct stat getInfos();
        std::string getIP();
        int getPort();
        std::string getRcon();
        std::streampos getRow();
        std::string getServerLog();
        int getStrict();
        bool isChanged();
        bool isValid();

        //setters
        void setName( std::string name );
        void setBackupDir( std::string backupDir );
        void setBotLog( std::string botLog );
        void setConfigFile( std::string configFile );
        void setDbFolder( std::string dbFolder );
        void setInfos( struct stat infos );
        void setIP( std::string ip );
        void setPort( int port );
        void setRcon( std::string rcon );
        void setRow( std::streampos row );
        void setServerLog( std::string serverLog );
        void setStrict( int level = 1 );
        void setChanged( bool changed = true );
        void setValid( bool valid = true );
        
        //funzioni per l'accesso diretto all'array dei giocatori
        unsigned int size();
        Player* operator[] ( int pos );    //overloading dell'operatore [int]
        void push_back( Player* player ); 
        std::vector<Server::Player*>::iterator begin();
        void erase( std::vector<Server::Player*>::iterator iteratore );
        void clear();
        
        //confronto con le vecchie opzioni, ed in caso di modifiche critiche lo resetta, altrimenti ripristina i dati dinamici.
        void test_for_changes(Server* old);
        //controlla se sono state date impostate tutte le opzioni, true=ok.
        bool test_for_options();
        
  private:
        bool m_changed;
        bool m_valid;
        std::string m_name;
        std::string m_configFile;   //file di cfg del bot (quella relativa al singolo server, con rcon e file)
        struct stat m_infos;    //contiene le info del file di configurazione, tra cui la data dell'ultima modifica
        std::string m_rconpass;
        std::string m_ip;
        int m_port;
        std::string m_backup;
        std::string m_botLog;
        std::string m_serverLog;
        std::string m_dbFolder;
        std::streampos m_row;
        int m_strict;           //livello di restrizioni.

        std::vector<Player*> m_giocatori;
};

#endif
