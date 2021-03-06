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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


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
#include "handyFunctions.h"

class ConfigLoader
{
    public:
        ConfigLoader( const std::string &filePath );
        ~ConfigLoader();

        class Options
        {
            public:
                struct stat infos;                              // has info about configuration file plus date of last modification
                std::string generalLog;
                std::string generalBackup;
                int generalPort;
                std::vector<Server*> servers;
                Logger* errors;                                 // pointer to log object used for notifications and errors
                Logger* log;                                    // pointer to log object dedicated to a server ( changes file )
                unsigned int serverNumber;                      // number of the server currently working on

                Server& operator[] ( int number ) { return *servers[number]; }
                unsigned int size() { return servers.size(); }  // returns number of servers

                //costruttore
                Options()
                : generalPort( 51337 )    
                , serverNumber( 0 )
                {
                    errors=new Logger();
                    log=new Logger();
                }
                //distruttore
                ~Options()
                {
                    delete errors;
                    delete log;
                };
                // return current server i'm working on (analyzer only)
                Server* currentServer() const
                {
                    return servers.at( serverNumber );
                }

                //distrugge gli oggetti contenuti in servers.
                void destroyServers()
                {
                    for( unsigned int i = 0; i < servers.size(); i++ )
                        delete servers[i];

                }
                //comoda funzione per cercare un server per nome.
                Server* searchServer(std::string name)
                {
                    Server* t = NULL;
                    for( unsigned int i = 0; i < servers.size(); i++ )
                        if ( name.compare( servers[i]->name() ) == 0 )
                            t = servers[i];
                        return t;
                };

                std::string toString ()
                {
                    std::string t;
                    t.append( "General log : " );
                    t.append( generalLog );
                    t.append( "\nGeneral backup : " );
                    t.append( generalBackup );
                    t.append( "\nServers number : " );
                    t.append( handyFunctions::intToString( servers.size() ) );
                    t.append( "\n" );
                    for (unsigned int i = 0; i < servers.size(); i++ )
                    {
                        t.append ( servers[i]->toString() );
                        t.append( "\n" );
                    }
                    return t;
                };
        };

        Options* getOptions();
        bool testChanges();
        void loadOptions();   //it reloads entire options tree, changes made on object Options*

    private:
        std::string generalFile;
        Options* opzioni;
        bool isA(const std::string &line, char* regex);
        std::string extract(const std::string &line);
};
#endif
