/*
    main.cpp is part of BanBot.

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
#include "Backup.h"

#define BOTCONFIG "cfg/BanBot.cfg"
#define BANLIST "cfg/Banlist"
#define ADMINLIST "cfg/Adminlist"

using namespace std;

int main( int argc, char *argv[] ){ //pass arguments to specify server logfile and bot logfile
    //carico le impostazioni e le salvo su opzioni
    ConfigLoader * caricatore = new ConfigLoader( BOTCONFIG );
    ConfigLoader * banList = new ConfigLoader( BANLIST );
    ConfigLoader * adminList = new ConfigLoader( ADMINLIST );

    std::vector<ConfigLoader::Option> opzioni = caricatore->getOptions();
    std::vector<ConfigLoader::Banlist> banned = banList->getBanlist();
    std::vector<ConfigLoader::Option> admins = adminList->getOptions();

    for ( int i = 0; i < opzioni.size(); i++ )
        cout << opzioni[i].name << " = " << opzioni[i].value << "\n";

    cout << "\n";   //extra "a-capo"

    delete caricatore;
    delete banList;
    delete adminList;

    //inizializzo il logger
    int i = 0;
    int found = 0;
    Logger *botLog = NULL;

    while( i < opzioni.size() && found == 0 ){
        if( opzioni[i].name.compare( "BOTLOG" ) == 0 ){
            botLog = new Logger( opzioni[i].value );
            found++;
        }
    }

    Db *d = new Db( opzioni, banned, admins, botLog ) ;

    found=0;
    vector<string> ip;
    vector<int> port;
    vector<string> password;
    vector<string> logpath;

    for(i=0;i < opzioni.size(); i++ ){
        if( opzioni[i].name.compare( "IP" ) == 0 ){
            ip.push_back( opzioni[i].value );
            found++;
        }
        else if( opzioni[i].name.compare( "PORT" ) == 0 ){
            port.push_back( atoi(opzioni[i].value.c_str()) );
            found++;
        }
        else if( opzioni[i].name.compare( "RCONPASS" ) == 0 ){
            password.push_back( opzioni[i].value );
            found++;
        }
        else if( opzioni[i].name.compare( "LOGPATH" ) == 0 ){
            logpath.push_back( opzioni[i].value );
            found++;
       }
       if (found==4) 
       {
         found=0;
       }
    }

    cout<<"[+] Importing values from config file:\n";
    for (int i=0;i<ip.size();i++)
    {
      cout<<"  [+] Server number "<<i<<":\n";
      cout<<"  [-] Ip of server: "<<ip[i]<<"\n";
      cout<<"  [-] Port of server: "<<port[i]<<"\n";
      cout<<"  [-] Rcon password: "<<password[i]<<"\n";
      cout<<"  [-] File di log: "<<logpath[i]<<"\n";
      cout<<"\n";
    }
    cout<<"\n";

    botLog->open();
    botLog->timestamp();
    *botLog<<"\n[+] Importing values from config file:\n";
    for (int i=0;i<ip.size();i++)
    {
      *botLog<<"  [+] Server number "<<i<<":\n";
      *botLog<<"  [-] Ip of server: "<<ip[i]<<"\n";
      *botLog<<"  [-] Port of server: "<<port[i]<<"\n";
      *botLog<<"  [-] Rcon password: "<<password[i]<<"\n";
      *botLog<<"  [-] File di log: "<<logpath[i]<<"\n";
      *botLog<<"\n";
    }
    *botLog<<"\n";

    //Start connection
    Connection *serverCommand = new Connection(opzioni);   //need vector with options

    Backup *back=new Backup(opzioni,botLog);
    //Start analyzer
    Analyzer anal( serverCommand, d,botLog, back, opzioni );
    anal.main_loop();



    /*string asd = "backup/dfghjk";
    if ( d->checkDirAndFile( asd ) )
        cout<<"true\n";
    else cout<<"false";*/
  //ho l'array con tutte le impostazioni

}
