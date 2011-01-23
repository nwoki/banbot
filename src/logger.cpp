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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


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

Logger::Logger( std::string p ): path( p ),isOpen( false )
{
    checkFile();
}

Logger::Logger():path( "" ),isOpen( false )
{
}

void Logger::checkFile()
{
  int end=path.find_last_of('/');
  int pos=0;
  bool ok=true;

  //check per la cartella
  while (pos<end && ok)
  {
    pos=path.find('/',pos+1);
    std::string cartella=path.substr(0,pos);

    struct stat st;
    if( stat( cartella.c_str(), &st ) != 0 )
    {
      if( mkdir( cartella.c_str(), 0777 ) != 0 )
      {
          std::cout<<"\e[1;31m[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions! \e[0m \n";
          ok=false;
      }
    }
  }

  //check per il file log
  if( ok )
  {
    struct stat st;
    if( stat( path.c_str(), &st ) != 0 )
    {
      std::cout<<"[!] Logfile doesn't exist... I'll create it.\n";
      //create logfile
      std::ofstream OUT(path.c_str());
      if ( OUT.is_open() ) OUT.close();
      else std::cout<<" \e[1;31m [EPIC FAIL] couldn't create the log file. Please check permissions! \e[0m \n";
    }
  }
}

Logger::~Logger()
{
    file.close();
}

void Logger::write(const char* valore)
{
  if ( isOpen || open() )
    file<<valore;
}

void Logger::write(const int valore)
{
  if ( isOpen || open() )
    file<<valore;
}

void Logger::changePath(std::string path)
{
    if (isOpen) close();
    this->path=path;
    checkFile();
}

bool Logger::open()
{
    file.open(path.c_str(),std::ios_base::app);
    isOpen=file.is_open();
    return isOpen;
}

void Logger::timestamp()
{
  if( isOpen || open() )
  {
    //stampo il timestamp e il messaggio
    time_t timestamp=time(NULL);
    std::string data(asctime(localtime(&timestamp)));
    data=data.substr(0,data.size()-1);
    file<<"\n["<<data<<"] ";  //asctime(localtime(&timestamp))
  }
}

void Logger::close()
{
  if (isOpen)
  {
    file.close();
    isOpen=false;
  }
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
