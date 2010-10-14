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

        class InstructionsBlock;
        class Player
        {
            public:
                std::string GUID;
                std::string number;
                std::string nick;
                std::string ip;

                Player()
                    : GUID("")
                    , number("")
                    , nick("")
                    , ip("")
                    {
                    };

                Player( std::string g, std::string nu, std::string ni, std::string i )
                    : GUID(g)
                    , number(nu)
                    , nick(ni)
                    , ip(i)
                    {
                    };
                Player* clone() { return new Player( GUID, number, nick, ip ); };
        };

        // getters
        std::string name() const;
        std::string backupDir() const;
        std::string botLog() const;
        std::string configFile() const;
        std::string dbFolder() const;
        struct stat infos() const;
        std::string ip() const;
        int port() const;
        std::string rcon() const;
        std::streampos row() const;
        std::string serverLog() const;
        int strict() const;
        bool isChanged() const;
        bool isValid() const;

        // setters
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

        // functions that access directly the players array
        unsigned int size();
        Player* operator[] ( int pos );    // overloading for the operator [int]
        void push_back( Player* player );
        std::vector<Server::Player*>::iterator begin();
        void erase( std::vector<Server::Player*>::iterator iteratore );
        void clear();

        //confronto con le vecchie opzioni, ed in caso di modifiche critiche lo resetta, altrimenti ripristina i dati dinamici.
        void test_for_changes(Server* old);
        //controlla se sono state date impostate tutte le opzioni, true=ok.
        bool test_for_options();


        std::string toString();

  private:
        bool m_changed;                     // flag that indicates when a server options have been modified
        bool m_valid;                       // flag that indicates wether the server is valid or not
        std::string m_name;                 // server name
        std::string m_configFile;           // bot config file( relative to the single server with rcon and file )
        struct stat m_infos;                // contains config file infotra cui la data dell'ultima modifica
        std::string m_rconpass;             // server rcon password
        std::string m_ip;                   // server ip
        int m_port;                         // server port
        std::string m_backup;               // server backup directory
        std::string m_botLog;               // server botlog directory
        std::string m_serverLog;            // log of the server to parse
        std::string m_dbFolder;             // server database folder directory
        std::streampos m_row;               // current row on log file
        int m_strict;                       // restriction level

        std::vector<Player*> m_giocatori;   // vector with the info of the players currently in the server
        InstructionsBlock* m_lowPriorityInst, *m_mediumPriorityInst, *m_highPriorityInst;   // instruction priorities
};

#endif
