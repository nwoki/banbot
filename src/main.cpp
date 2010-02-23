/*
    This file is part of BanBot.

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

#include <iostream>
#include <string>
#include <vector>
#include "Analyzer.h"
#include "db.h"
#include "connection.h"
#include "ConfigLoader.h"
#include "sqlite3/sqlite3.h"
#include "logger.h"

#define CONFIG "BanBot.cfg"

using namespace std;

int main( int argc, char *argv[] ){ //pass arguments to specify server logfile and bot logfile
    //carico le impostazioni e le salvo su opzioni
    ConfigLoader * caricatore;
    if ( argc == 2 )
        caricatore = new ConfigLoader(argv[1]);
    else
        caricatore = new ConfigLoader( CONFIG );
    std::vector<ConfigLoader::Option> opzioni = caricatore->getOptions();
    for (int i=0;i<opzioni.size();i++)
    {
      cout<<opzioni[i].name<<" = "<<opzioni[i].value<<"\n";
    }
    cout<<"\n";
    delete caricatore;
    
    //inizializzo il logger
    int i=0;
    int founded=0;
    Logger * botLog=NULL;
    while(i<opzioni.size() && founded==0)
    {
      if (opzioni[i].name.compare("BOTLOG")==0)
      {
	botLog=new Logger(opzioni[i].value);
	founded++;
      }
    }

    Db *d = new Db(opzioni,botLog) ;

    founded=0;
    string ip;
    int port=0;
    string password;
    string logpath;
    for (i=0;founded<4 && i<opzioni.size();i++)
    {
      if (opzioni[i].name.compare("IP")==0)
      {
	ip=opzioni[i].value;
	founded++;
      }
      else if (opzioni[i].name.compare("PORT")==0)
      {
	port=atoi(opzioni[i].value.c_str());
	founded++;
      }
      else if (opzioni[i].name.compare("RCONPASS")==0)
      {
	password=opzioni[i].value;
	founded++;
      }
      else if (opzioni[i].name.compare("LOGPATH")==0)
      {
	logpath=opzioni[i].value;
	founded++;
      }
    }
    
    cout<<"[+] Importing values from config file:\n";
    cout<<"  [-] Ip of server: "<<ip<<"\n";
    cout<<"  [-] Port of server: "<<port<<"\n";
    cout<<"  [-] Rcon password: "<<password<<"\n";
    cout<<"  [-] File di log: "<<logpath<<"\n";
    
    botLog->open();
    botLog->timestamp();
    *botLog<<"\n[+] Importing values from config file:\n"<<"  [-] Ip of server: "<<ip<<"\n"<<"  [-] Port of server: "<<port<<"\n"
    <<"  [-] Rcon password: "<<password<<"\n"<<"  [-] File di log: "<<logpath<<"\n";

    Connection *serverCommand = new Connection(ip,port,password);

    Analyzer anal( serverCommand, d,botLog, logpath );
    botLog->close();
    anal.main_loop();


  //ho l'array con tutte le impostazioni

}
