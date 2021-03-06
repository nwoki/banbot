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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/
#include <iostream>
#include <string.h>
#include <vector>
#include "Analyzer.h"
#include "db.h"
#include "connection.h"
#include "ConfigLoader.h"
#include "sqlite3/sqlite3.h"
#include "logger.h"

#define BOTCONFIG "cfg/BanBot.cfg"
#define BANLIST "cfg/Banlist"
#define ADMINLIST "cfg/Adminlist"

#define _VERSION "1.2b1"

int main( int argc, char *argv[] )
{
    //pass argument to specify bot configfile
    //pass -c to convert databases

    #ifdef ITA
        std::cout<<"BanBot versione"<<_VERSION<<", un progetto open-source di [2s2h]n3m3s1s and [2s2h]Zamy,\n   Copyright © 2010, 2011\n";
        std::cout<<"per scopi legali, guarda la GNU General Public License:  <http://www.gnu.org/licenses/>\n\n";
        std::cout<<"Attendere, inizio a caricare questa merda...\n";
    #else
        std::cout<<"BanBot version"<<_VERSION<<", an open-source project by [2s2h]n3m3s1s and [2s2h]Zamy,\n   Copyright © 2010, 2011\n";
        std::cout<<"look at GNU General Public License for legal purposes:  <http://www.gnu.org/licenses/>\n\n";
        std::cout<<"Wait, pushing up this shit...\n";
    #endif

    //load parameters and options, and save them in "opzioni"
    ConfigLoader * caricatore = NULL;
    bool convert = false;
    if ( argc > 1 )
        for ( int i=1; i<argc; i++ )
        {
            if ( strcmp(argv[i],"-c") == 0 )
                convert = true;
            else
                caricatore = new ConfigLoader(argv[i]);
        }
    if (caricatore == NULL)
        caricatore = new ConfigLoader( BOTCONFIG );

    if ( caricatore->getOptions() == NULL )
    {
        #ifdef ITA
            std::cout<<"[EPIC FAIL] File di configurazione principale non trovato. Wtf?!?!?\n";
        #else
            std::cout<<"[EPIC FAIL] Main config file not found. Wtf?!?!?\n";
        #endif
    }
    else
    {
        std::cout << caricatore->getOptions()->toString();
        std::cout << "\n";   //extra "a-capo"
        //inizialize db
        Db *d = new Db( caricatore->getOptions());
        //convert databases
        bool run = true;
        if ( convert )
            run = d->sequentialDbUpgrade();
        if (run)
        {
            //Start connection
            Connection *serverCommand = new Connection(caricatore->getOptions());   //need std::vector with options

            //Start analyzer
            Analyzer anal( serverCommand, d, caricatore );
            #ifdef ITA
                std::cout<<"Inizio l'elaborazione tra 5 secondi...\n";
            #else
                std::cout<<"Start in 5 seconds...\n";
            #endif
            sleep (5);
            anal.main_loop();
        }
        else
            std::cout<<"Error converting databases. Stop.\n";
    }

}
