/*
    logger.cpp is part of BanBot.

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

#ifndef _logger_cpp_
#define _logger_cpp_

#include "logger.h"
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

Logger::Logger( std::string p ): path( p )
{
    size_t pos=path.find_last_of('/');
    std::string cartella=path.substr(0,pos);
    //std::string file=path.substr(pos+1);

    //check per la cartella
    struct stat st;
    if( stat( cartella.c_str(), &st ) == 0 )
    {
        std::cout<<"  [*]dir '"<<cartella<<"/' found\n";
    }
    else
    {
        std::cout<<"  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";

        std::string command("mkdir \"");
        command.append(cartella);
        command.append("\"");
        if( !system(command.c_str()))
        {
            std::cout<<"  [OK]created '"<<cartella<<"/' directory..\n";
        }
        else
        {
            std::cout<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
        }
    }
    //check per il file log
    std::cout<<"[-] checking for logfile..\n";
    std::ifstream IN( path.c_str() );
    if( IN.is_open() ) IN.close();
    else
    {
        std::cout<<"[!] Logfile doesn't exist... I'll create it.\n";
        //create logfile
        std::ofstream OUT(path.c_str());
        if ( OUT.is_open() ) OUT.close();
        else std::cout<<"[ERR] couldn't create the log file.Please check permissions!\n";
    }
}

Logger::Logger():path("")
{
}

Logger::~Logger()
{
    file.close();
}

void Logger::write(const char* valore)
{
    file<<valore;
}

void Logger::write(const int valore)
{
    file<<valore;
}

void Logger::changePath(std::string path)
{
    this->path=path;
}

bool Logger::open()
{
    file.open(path.c_str(),std::ios_base::app);
    return file.is_open();
}

void Logger::timestamp()
{
    //stampo il timestamp e il messaggio
    time_t timestamp=time(NULL);
    std::string data(asctime(localtime(&timestamp)));
    data=data.substr(0,data.size()-1);
    file<<"\n["<<data<<"] ";  //asctime(localtime(&timestamp))
}

void Logger::close()
{
    file.close();
}

//funzioni ESTERNE alla classe,di comodo
Logger& operator<<(Logger& l,const char* valore)
{
    l.write(valore);
    return l;
}
Logger& operator<<(Logger& l,std::string valore)
{
    l.write(valore.c_str());
    return l;
}
Logger& operator<<(Logger& l,const int valore)
{
    l.write(valore);
    return l;
}
#endif
