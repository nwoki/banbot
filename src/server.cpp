/*
    server.cpp is part of BanBot.

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

#ifndef _server_h_
#define _server_h_

#include "server.h"

Server::~Server()
{
  for (int i=0;i<giocatori.size();i++) 
    delete giocatori[i];
}

Server::Player* Server::operator[] (int number)
{
  return giocatori[number];
}

std::streampos Server::getRow()
{
  return row;
}
void Server::setRow( std::streampos pos )
{
  row=pos;
}

std::string Server::getConfigFile()
{
  return file;
}
void Server::setConfigFile(std::string configFile)
{
  file=configFile;
}

std::string Server::getRcon()
{
  return rconpass;
}
void Server::setRcon(std::string rconPassword)
{
  rconpass=rconPassword;
}

std::string Server::getIP()
{
  return ip;
}
void Server::setIP(std::string ip)
{
  this->ip=ip;
}

std::string Server::getPort()
{
  return port;
}
void Server::setPort(std::string port)
{
  this->port=port;
}

std::string Server::getBackupDir()
{
  return backup;
}
void Server::setBackupDir(std::string backupDir)
{
  backup=backupDir;
}

std::string Server::getBotLog()
{
  return botlog;
}
void Server::setBotLog(std::string botLog)
{
  botlog=botLog;
}

std::string Server::getServerLog()
{
  return serverlog;
}
void Server::setServerLog(std::string serverLog)
{
  serverlog=serverLog;
}

std::string Server::getDbFolder()
{
  return dbfolder;
}
void Server::setDbFolder(std::string dbFolder)
{
  dbfolder=dbFolder;
}

struct stat Server::getInfos()
{
  return infos;
}
void Server::setInfos(struct stat fileInfos)
{
  infos=fileInfos;
}

#endif
