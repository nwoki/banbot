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
    //just empty so class compiles
}

Server::~Server()
{
    for( unsigned int i = 0; i < m_giocatori.size(); i++ )
        delete m_giocatori[i];
}

Server::Player* Server::operator[] ( int pos )
{
    return m_giocatori[pos];
}

std::streampos Server::getRow()
{
    return m_row;
}

void Server::setRow( std::streampos pos )
{
    m_row = pos;
}

std::string Server::getConfigFile()
{
    return m_configFile;
}

void Server::setConfigFile( std::string configFile )
{
    m_configFile = configFile;
}

std::string Server::getRcon()
{
    return m_rconpass;
}

void Server::setRcon( std::string rconPassword )
{
    m_rconpass = rconPassword;
}

std::string Server::getIP()
{
    return m_ip;
}

void Server::setIP( std::string ip )
{
    m_ip = ip;
}

std::string Server::getPort()
{
    return m_port;
}

void Server::setPort( std::string port )
{
    m_port = port;
}

std::string Server::getBackupDir()
{
    return m_backup;
}

void Server::setBackupDir( std::string backupDir )
{
    m_backup = backupDir;
}

std::string Server::getBotLog()
{
    return m_botlog;
}

void Server::setBotLog( std::string botLog )
{
    m_botlog = botLog;
}

std::string Server::getServerLog()
{
    return m_serverlog;
}

void Server::setServerLog( std::string serverLog )
{
    m_serverlog = serverLog;
}

std::string Server::getDbFolder()
{
    return m_dbfolder;
}

void Server::setDbFolder( std::string dbFolder )
{
    m_dbfolder = dbFolder;
}

struct stat Server::getInfos()
{
    return m_infos;
}

void Server::setInfos( struct stat fileInfos )
{
    m_infos = fileInfos;
}

#endif
