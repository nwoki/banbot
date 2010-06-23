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
    bool changed;
    std::string file;                  //file di cfg del bot (quella relativa al singolo server, con rcon e file)
    struct stat infos;                 //contiene le info del file di configurazione, tra cui la data dell'ultima modifica
    std::string rconpass;
    std::string ip;
    std::string port;
    std::string backup;
    std::string botlog;
    std::string serverlog;
    std::string dbfolder;
    std::streampos row;
    
    class Player
    {
        public:
        std::string GUID;
        std::string number;
        std::string nick;
        std::string ip;
    };
    
    std::vector<Player*> giocatori;
    
  public:
    Server();
    Player* operator[] (int);    //overloading dell'operatore [int]
    std::streampos getRow();
    void setRow( std::streampos );
    std::string getConfigFile();
    void setConfigFile(std::string);
    std::string getRcon();
    void setRcon(std::string);
    std::string getIP();
    void setIP(std::string);
    std::string getPort();
    void setPort(std::string);
    std::string getBackupDir();
    void setBackupDir(std::string);
    std::string getBotLog();
    void setBotLog(std::string);
    std::string getServerLog();
    void setServerLog(std::string);
    std::string getDbFolder();
    void setDbFolder(std::string);
    struct stat getInfos();
    void setInfos(struct stat);
};

#endif
