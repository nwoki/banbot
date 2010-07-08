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

#ifndef _server_cpp_
#define _server_cpp_

#include "server.h"

Server::Server()
{
  m_port=0;
  m_changed=true;
  m_valid=true;
  m_strict=0;
}

Server::~Server()
{
    for( unsigned int i = 0; i < m_giocatori.size(); i++ )
        delete m_giocatori[i];
}


std::string Server::name() const
{
  return m_name;
}

void Server::setName( std::string name )
{
  m_name=name;
}


std::streampos Server::row() const
{
    return m_row;
}

void Server::setRow( std::streampos pos )
{
    m_row = pos;
}


std::string Server::configFile() const
{
    return m_configFile;
}

void Server::setConfigFile( std::string configFile )
{
    m_configFile = configFile;
}


std::string Server::rcon() const
{
    return m_rconpass;
}

void Server::setRcon( std::string rconPassword )
{
    m_rconpass = rconPassword;
}


std::string Server::ip() const
{
    return m_ip;
}

void Server::setIP( std::string ip )
{
    m_ip = ip;
}


int Server::port() const
{
    return m_port;
}

void Server::setPort( int port )
{
    m_port = port;
}


std::string Server::backupDir() const
{
    return m_backup;
}

void Server::setBackupDir( std::string backupDir )
{
    m_backup = backupDir;
}


std::string Server::botLog() const
{
    return m_botLog;
}

void Server::setBotLog( std::string botLog )
{
    m_botLog = botLog;
}


std::string Server::serverLog() const
{
    return m_serverLog;
}

void Server::setServerLog( std::string serverLog )
{
    m_serverLog = serverLog;
}


std::string Server::dbFolder() const
{
    return m_dbFolder;
}

void Server::setDbFolder( std::string dbFolder )
{
    m_dbFolder = dbFolder;
}


struct stat Server::infos() const
{
    return m_infos;
}

void Server::setInfos( struct stat fileInfos )
{
    m_infos = fileInfos;
}


int Server::strict() const
{
  return m_strict;
}

void Server::setStrict( int level )
{
  m_strict=level;
}


bool Server::isChanged() const
{
  return m_changed;
}

void Server::setChanged( bool changed )
{
  m_changed=changed;
}


bool Server::isValid() const
{
  return m_valid;
}

void Server::setValid( bool valid )
{
  m_valid=valid;
}

//**************************************** funzioni per l'accesso diretto all'array di giocatori ************************
unsigned int Server::size()
{
  return m_giocatori.size();
}

Server::Player* Server::operator[] ( int pos )
{
    return m_giocatori[pos];
}

void Server::push_back( Player* player )
{
  m_giocatori.push_back( player );
}

std::vector<Server::Player*>::iterator Server::begin()
{
  return m_giocatori.begin();
}

void Server::erase( std::vector<Server::Player*>::iterator iteratore)
{
  m_giocatori.erase( iteratore );
}

void Server::clear()
{
  m_giocatori.clear();
}


//************************************* altro ***********************

void Server::test_for_changes(Server* old)
{
  if ( old != NULL && old != 0)
  {
    if ( ( !old->m_valid ) || ( m_serverLog.compare(old->m_serverLog) != 0 ) || ( m_dbFolder.compare(old->m_dbFolder) != 0 ) )
    {
      //cambiamenti importanti. Non tengo niente dei vecchi dati.
      m_changed = true;
    }
    else
    {
      //nessun cambiamento importante: mi prendo i dati del server vecchio.
      //copio i giocatori
      for (unsigned int i=0; i<old->size(); i++ )
      {
        m_giocatori.push_back( (*old)[i]->clone() );
      }
      //e ripristino anche gli altri parametri dinamici
      m_strict = old->m_strict;
      m_row = old->m_row;
    }
  }
  else
  {
    m_changed = true;
  }
}

bool Server::test_for_options()
{
  if ( !m_name.empty() && !m_rconpass.empty() && !m_ip.empty() && m_port!=0 && !m_backup.empty() && !m_botLog.empty() && !m_serverLog.empty() && !m_dbFolder.empty() )
    return true;
  return false;
}

#endif
