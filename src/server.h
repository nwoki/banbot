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
    private:
        bool m_changed;
        std::string m_configFile;   //file di cfg del bot (quella relativa al singolo server, con rcon e file)
        struct stat m_infos;    //contiene le info del file di configurazione, tra cui la data dell'ultima modifica
        std::string m_rconpass;
        std::string m_ip;
        std::string m_port;
        std::string m_backup;
        std::string m_botlog;
        std::string m_serverlog;
        std::string m_dbfolder;
        std::streampos m_row;

    class Player
    {
        public:
            std::string GUID;
            std::string number;
            std::string nick;
            std::string ip;
    };

    std::vector<Player*> m_giocatori;

    public:
        Server();
        ~Server();

        Player* operator[] ( int pos );    //overloading dell'operatore [int]

        //getters
        std::string getBackupDir();
        std::string getBotLog();
        std::string getConfigFile();
        std::string getDbFolder();
        struct stat getInfos();
        std::string getIP();
        std::string getPort();
        std::string getRcon();
        std::streampos getRow();
        std::string getServerLog();

        //setters
        void setBackupDir( std::string backupDir );
        void setBotLog( std::string botLog );
        void setConfigFile( std::string configFile );
        void setDbFolder( std::string dbFolder );
        void setInfos( struct stat infos );
        void setIP( std::string ip );
        void setPort( std::string port );
        void setRcon( std::string rcon );
        void setRow( std::streampos row );
        void setServerLog( std::string serverLog );
};

#endif
