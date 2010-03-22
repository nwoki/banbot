/*
 *
 *   	Backup.cpp is part of BanBot.
 *
 *   	BanBot is freesoftware: you can redistribute it and/or modify
 *   	it under the terms of the GNU General Public License as published by
 *   	the Free Software Foundation, either version 3 of the License, or
 *   	(at your option) any later version.
 *
 *	BanBot is distributed in the hope that it will be useful,
 *   	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   	GNU General Public License for more details.
 *
 *   	You should have received a copy of the GNU General Public License
 *   	along with BanBot (look at GPL_License.txt).
 *   	If not, see <http://www.gnu.org/licenses/>.
 *
 *   	Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)
 *
 *
 *   	BanBot uses SQLite3:
 *   	Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
 *   	Software Foundation, Inc.
 *
 */
#ifndef _backup_cpp_
#define _backup_cpp_
#include "Backup.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>


#include <iostream>

Backup::Backup(std::vector<ConfigLoader::Option> opzioni)
{
  for (unsigned int i=0;i<opzioni.size();i++)
  {
    if (opzioni[i].name.compare("LOGPATH")==0 || opzioni[i].name.compare("BOTLOG")==0 || opzioni[i].name.compare("BOTLOGPATH"))
    {
      files.push_back(opzioni[i].value);
    }
    else if (opzioni[i].name.compare("BACKUPPATH")==0)
    {
      directory=opzioni[i].value;
    }
  }
  done=false;
}

Backup::~Backup()
{
}

void Backup::checkFolder(std::string path)
{
  //controllo se esiste la directory: se non esiste la creo.
  struct stat st;
  if (stat(path.c_str(),&st))
  {
    mkdir(path.c_str(),0777);
  }
}

bool Backup::isTimeToWork()
{
  time_t tempo;
  tempo=time(NULL);
  struct tm *tmp;
  tmp = localtime(&tempo);
  char outstr[20];
  strftime(outstr, sizeof(outstr), "%H:%M", tmp);
  std::string attuale(outstr);
  std::string ofWork(HOUR);
  ofWork.append(":");
  std::string ofWork2=ofWork;
  ofWork.append(MINUTE);
  ofWork2.append(OVERMINUTE);
  std::cout<<attuale<<" "<<ofWork<<" "<<ofWork2<<" "<<done<<"\n";
  if (attuale.compare(ofWork)>=0 && attuale.compare(ofWork2)<=0)
  {
    if (!done)
    {
      done=true;
      return true;
    }
    return false;
  }
  else
  {
     done=false;
     return false;
  }
}

void Backup::spostaFiles()
{
  //creo la cartella con la data.
  //prendo la data nel formato yyyy-mm-dd
  char outstr[11];
  time_t t;
  struct tm *tmp;
  t = time(NULL);
  tmp = localtime(&t);
  strftime(outstr, sizeof(outstr), "%F", tmp);
  //ho la data salvata in outstr, creo la cartella.
  std::string cartella=directory;
  cartella.append("/");
  cartella.append(outstr);
  checkFolder(cartella);
  //sposto i files.
  for (int i=0; i<files.size(); i++)
  {
    //mi preparo la stringa con il file di destinazione
    int pos=files[i].find_last_of("/");
    if (pos==-1) pos=0;
    std::cout<<"Dio cane: "<<files[i]<<" "<<pos;
    std::string nomeFile=cartella;
    nomeFile.append("/");
    nomeFile.append(files[i].substr(pos));
    //sposto il file
    std::rename(files[i].c_str(),nomeFile.c_str());
  }
}

void Backup::doJobs()
{
  if (isTimeToWork())
  {
    checkFolder(directory);
    spostaFiles();
  }
}

#endif