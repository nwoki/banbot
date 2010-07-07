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

#define BOTCONFIG "cfg/BanBot.cfg"
#define BANLIST "cfg/Banlist"
#define ADMINLIST "cfg/Adminlist"

using namespace std;

int main( int argc, char *argv[] )
{ 
    //pass argument to specify bot configfile

    cout<<"BanBot version 1.1b, an open-source project by [2s2h]n3m3s1s and [2s2h]Zamy,\n   Copyright © 2010\n";
    cout<<"look at GNU General Public License for legal purposes:  <http://www.gnu.org/licenses/>\n\n";
    cout<<"Wait, pushing up this shit...\n";

    //carico le impostazioni e le salvo su opzioni
    ConfigLoader * caricatore;
    if ( argc == 2 )
        caricatore = new ConfigLoader(argv[1]);
    else
        caricatore = new ConfigLoader( BOTCONFIG );

    if ( caricatore->getOptions() == NULL )
    {
        cout<<"[EPIC FAIL] Main config file not found. Wtf?!?!?";
    }
    else
    {
        cout << "\n";   //extra "a-capo"
        //inizializzo db
        Db *d = new Db( caricatore->getOptions()) ;
        
        
        //Start connection
        Connection *serverCommand = new Connection(caricatore->getOptions());   //need vector with options
        
        //Start analyzer
        Analyzer anal( serverCommand, d, caricatore );
        anal.main_loop();
    }

}
