/*
    ConfigLoader.h is part of BanBot.

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

#ifndef _ConfigLoader_h_
#define _ConfigLoader_h_

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <time.h>
#include "server.h"
#include "logger.h"

class ConfigLoader
{
  public:
    ConfigLoader( const std::string &filePath );
    ~ConfigLoader();
    
    class Options
    {
      public:
        bool changed;
        struct stat infos;                 //contiene le info del file di configurazione, tra cui la data dell'ultima modifica
        std::string generalLog;
        std::string generalBackup;
        std::vector<Server> servers;
        Logger* errors;                     //log dedicato a notifiche ed errori
        Logger* log;                        //log dedicato ai server (cambia di file)
        unsigned int serverNumber;                   //numero del server su cui sto lavorando
        
        Server operator[] (int number){return servers[number];}
        unsigned int size(){return servers.size();}
    };
    
    class AdminList    //use for adminlist as well
    {
    public:
        std::string name;
        std::string value;
    };

    class Banlist
    {
    public:
        std::string nick, ip, date, time, motive, author;
        std::vector< std::string > guids;
    };

    Options* getOptions();
    std::vector< ConfigLoader::AdminList > getAdminlist();
    std::vector< ConfigLoader::Banlist > getBanlist();
    bool testChanges();
    void reloadOptions();   //it reloads entire options tree, changes made on object Options*
    
  private:
    std::string generalFile;
    Options* opzioni;
};
#endif
