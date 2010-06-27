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

Backup::Backup(ConfigLoader::Options* opzioni)
  : done(false)
{
  /*for (unsigned int i=0;i<opzioni.size();i++)
  {
    if (opzioni[i].name.compare("LOGPATH")==0 || opzioni[i].name.compare("BOTLOG")==0 || opzioni[i].name.compare("BOTLOGPATH")==0)
    {
      files.push_back(opzioni[i].value);
    }
    else if (opzioni[i].name.compare("BACKUPPATH")==0)
    {
      directory=opzioni[i].value;
    }
  }*/
}

Backup::~Backup()
{
}

void Backup::creaCartelle()
{
  //controllo le cartelle dei server
  for (unsigned int i=0; i<m_options->size(); i++ )
  {
    if ( (*m_options)[i].isChanged() )
    {
      std::cout<<"\nInizializzo il sistema di backup di "<< (*m_options)[i].getName() <<" ...\n";
      std::string dir ( (*m_options)[i].getBackupDir() );
      unsigned int pos=0;
      bool ok=true;
      
      //check per la cartella
      while (pos<dir.size()-1 && ok)
      {
        pos=dir.find('/',pos+1);
        std::string cartella=dir.substr(0,pos);

        struct stat st;
        if( stat( cartella.c_str(), &st ) == 0 )
        {
            std::cout<<"  [*]dir '"<<cartella<<"/' found\n";
        }
        else
        {
          std::cout<<"   [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";

          if( mkdir( cartella.c_str(), 0777 ) == 0 )
          {
              std::cout<<"  [OK]created '"<<cartella<<"/' directory..\n";
          }
          else
          {
              std::cout<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
              *(m_options->errors)<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/' for " << (*m_options)[i].getName() << ". Please check permissions!\n";
              ok=false;
          }
        }
      }
    }
  }
  
  //controllo la cartella di backup generale
  if ( m_options->changed )
  {
    std::cout<<"\nInizializzo il sistema di backup generale ...\n";
    std::string dir ( m_options->generalBackup );
    unsigned int pos=0;
    bool ok=true;
    
    //check per la cartella
    while (pos<dir.size()-1 && ok)
    {
      pos=dir.find('/',pos+1);
      std::string cartella=dir.substr(0,pos);

      struct stat st;
      if( stat( cartella.c_str(), &st ) == 0 )
      {
          std::cout<<"  [*]dir '"<<cartella<<"/' found\n";
      }
      else
      {
        std::cout<<"   [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";

        if( mkdir( cartella.c_str(), 0777 ) == 0 )
        {
            std::cout<<"  [OK]created '"<<cartella<<"/' directory..\n";
        }
        else
        {
            std::cout<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
            *(m_options->errors)<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/' for general backup. Please check permissions!\n";
            ok=false;
        }
      }
    }
  }
}

void Backup::checkFolder(std::string path)
{
  //controllo se esiste la directory: se non esiste la creo.
  struct stat st;
  if (stat(path.c_str(),&st))
  {
    if ( mkdir(path.c_str(),0777) !=0 )
    {
      *(m_options->errors)<<"[EPIC FAIL] couldn't create directory '"<<path<<"/'. Please check permissions!\n";
    }
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
  if (attuale.compare(ofWork)>=0 && attuale.compare(ofWork2)<=0)
  {
    if (!done)
    {
      done=true;
      return true;
    }
  }
  else
  {
     done=false;
  }
  return false;
}

void Backup::spostaFiles( unsigned int server )
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
  std::string cartella=(*m_options)[server].getBackupDir();
  if(cartella.substr(cartella.size()-1).compare("/")!=0) cartella.append("/");
  cartella.append(outstr);
  checkFolder(cartella);
  //sposto i files.
  
  //BotLog
    //mi preparo la stringa con il file di destinazione
    int pos=(*m_options)[server].getBotLog().find_last_of("/");
    if (pos==-1) pos=0;
    else pos++;
    std::string nomeFile=cartella;
    nomeFile.append("/");
    nomeFile.append((*m_options)[server].getBotLog().substr(pos));
    //sposto il file
    std::cout<<" sposto "<<(*m_options)[server].getBotLog()<<" in "<<nomeFile<<".\n";
    *(m_options->errors)<<" sposto "<<(*m_options)[server].getBotLog()<<" in "<<nomeFile<<".\n";
    std::rename((*m_options)[server].getBotLog().c_str(),nomeFile.c_str());
    
  //ServerLog
    //mi preparo la stringa con il file di destinazione
    pos=(*m_options)[server].getServerLog().find_last_of("/");
    if (pos==-1) pos=0;
    else pos++;
    nomeFile.clear();
    nomeFile=cartella;
    nomeFile.append("/");
    nomeFile.append((*m_options)[server].getServerLog().substr(pos));
    //sposto il file
    std::cout<<" sposto "<<(*m_options)[server].getServerLog()<<" in "<<nomeFile<<".\n";
    *(m_options->errors)<<" sposto "<<(*m_options)[server].getServerLog()<<" in "<<nomeFile<<".\n";
    std::rename((*m_options)[server].getServerLog().c_str(),nomeFile.c_str());
}

void Backup::spostaFilesGenerali()
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
  std::string cartella=m_options->generalBackup;
  if(cartella.substr(cartella.size()-1).compare("/")!=0) cartella.append("/");
  cartella.append(outstr);
  checkFolder(cartella);
  //sposto i files.
  
  //GeneralLog
    //mi preparo la stringa con il file di destinazione
    int pos=m_options->generalLog.find_last_of("/");
    if (pos==-1) pos=0;
    else pos++;
    std::string nomeFile=cartella;
    nomeFile.append("/");
    nomeFile.append(m_options->generalLog.substr(pos));
    //sposto il file
    std::cout<<" sposto "<<m_options->generalLog<<" in "<<nomeFile<<".\n";
    *(m_options->errors)<<" sposto "<<m_options->generalLog<<" in "<<nomeFile<<".\n";
    (m_options->errors)->close();
    std::rename(m_options->generalLog.c_str(),nomeFile.c_str());
}

bool Backup::doJobs()
{
  if (isTimeToWork())
  {
    std::cout<<"Inizio il backup...";
    (m_options->errors)->timestamp();
    *(m_options->errors)<<"\nInizio il backup...";
    
    //faccio il backup per tutti i server...
    for (unsigned int i=0; i<m_options->size(); i++)
    {
      spostaFiles(i);
    }
    
    std::cout<<"Finito.\n";
    (m_options->errors)->timestamp();
    *(m_options->errors)<<"\nBackup eseguito.\n\n";
    return true;
  }
  return false;
}

#endif