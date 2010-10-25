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

class InstructionsBlock;

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

        enum PriorityLevel {
            LOW,
            MEDIUM,
            HIGH
        };

        // getters
        std::string name() const;                                           // returns server name
        std::string backupDir() const;                                      // returns path to server backup directory
        std::string botLog() const;                                         // returns path to server bot log file
        std::string configFile() const;                                     // returns path to server config file
        std::string dbFolder() const;                                       // returns path to server database folder
        struct stat infos() const;                                          // returns server infos struct
        std::string ip() const;                                             // returns server ip
        int port() const;                                                   // returns server port
        std::string rcon() const;                                           // returns server rcon pass
        std::streampos row() const;                                         // returns current row on log file
        std::string serverLog() const;                                      // returns server log path
        int strict() const;                                                 // returns strict level
        bool isChanged() const;                                             // checks if server options have been changed
        bool isValid() const;                                               // checks if server is valid
        InstructionsBlock* priorityInstrBlock( Server::PriorityLevel lvl ); // returns pointer to InstructionBlock according to level specified

        // setters
        void setName( std::string name );                                   // set server name
        void setBackupDir( std::string backupDir );                         // set server backup directory
        void setBotLog( std::string botLog );                               // set server botlog file
        void setConfigFile( std::string configFile );                       // set server ConfigFile
        void setDbFolder( std::string dbFolder );                           // set server database folder
        void setInfos( struct stat infos );                                 // set server infos struct
        void setIP( std::string ip );                                       // set server ip
        void setPort( int port );                                           // set server port
        void setRcon( std::string rcon );                                   // set server rcon password
        void setRow( std::streampos row );                                  // set server log row
        void setServerLog( std::string serverLog );                         // set server server log file
        void setStrict( int level = 1 );                                    // set server strict level
        void setChanged( bool changed = true );                             // set server changed flag
        void setValid( bool valid = true );                                 // set server validity flag
        void addPriorityInstrBlock( Server::PriorityLevel lvl, InstructionsBlock *inst );   // ADDS TO TAIL given InstructionsBlock
        void setPriorityInstrBlock( Server::PriorityLevel lvl, InstructionsBlock *inst );   // SUBSTITUTES current InstructionsBlock with the given one

        // functions that access directly the players array
        unsigned int size();                // returns the size of the player container
        Player* operator[] ( int pos );     // overloading for the operator [int]
        void push_back( Player* player );   // adds player to container
        std::vector<Server::Player*>::iterator begin();
        void erase( std::vector<Server::Player*>::iterator iteratore ); // erases given player from container
        void clear();                       // clears players container

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
        InstructionsBlock *m_lowPriorityInst, *m_mediumPriorityInst, *m_highPriorityInst;   // instruction priorities
};

#endif
