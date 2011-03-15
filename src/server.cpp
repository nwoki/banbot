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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/

#ifndef _server_cpp_
#define _server_cpp_

#include <time.h>
#include "InstructionsBlock.h"
#include "server.h"

Server::Server()
    : m_changed( true )
    , m_valid( true )
    , m_ip( "127.0.0.1" )
    , m_port( 0 )
    , m_strict( 0 )
    , m_warnings( PRIVATE )
    , m_banNick( NEVER )
    , m_banIp( NEVER )
    , m_banWarnings( true )
    , m_lastBalance( "" )
    , m_instructionCounter( new InstructionCounter() )
    , m_lowPriorityInst( NULL )
    , m_mediumPriorityInst( NULL )
    , m_highPriorityInst( NULL )
    , m_configs( std::vector<std::string>() )
    , m_maps( std::vector<std::string>() )
    , m_permissions( std::vector<int>() )
{
    //initializing command permissions' vector (default power levels needed)
    //group 0 = full admin
    //group 1 = admin
    //group 2 = clan member
    //group 3 = clan friends
    //group 100 = others (all)
    m_permissions.push_back( 100 );   //help level
    m_permissions.push_back( 2 );     //kick level
    m_permissions.push_back( 1 );     //ban level
    m_permissions.push_back( 1 );     //unban level
    m_permissions.push_back( 3 );     //mute level
    m_permissions.push_back( 1 );     //op level
    m_permissions.push_back( 1 );     //deop level
    m_permissions.push_back( 0 );     //strict level
    m_permissions.push_back( 3 );     //nuke level
    m_permissions.push_back( 3 );     //slap level
    m_permissions.push_back( 3 );     //veto level
    m_permissions.push_back( 3 );     //force level
    m_permissions.push_back( 2 );     //map level
    m_permissions.push_back( 3 );     //nextmap level
    m_permissions.push_back( 3 );     //admins level
    m_permissions.push_back( 0 );     //pass level
    m_permissions.push_back( 1 );     //config level
    m_permissions.push_back( 100 );   //status level
    m_permissions.push_back( 2 );     //warnings level
    m_permissions.push_back( 2 );     //bantimewarn level
    m_permissions.push_back( 3 );     //restart level
    m_permissions.push_back( 2 );     //reload level
    m_permissions.push_back( 3 );     //teambalance level
    m_permissions.push_back( 2 );     //gravity level
    m_permissions.push_back( 1 );     //changelevel level
}

Server::~Server()
{
    for( unsigned int i = 0; i < m_giocatori.size(); i++ )
        delete m_giocatori[i];
    if (m_lowPriorityInst!=NULL) m_lowPriorityInst->deleteAll();
    if (m_mediumPriorityInst!=NULL) m_mediumPriorityInst->deleteAll();
    if (m_highPriorityInst!=NULL) m_highPriorityInst->deleteAll();
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

InstructionsBlock* &Server::priorityInstrBlock( Server::PriorityLevel lvl )
{
    if( lvl == Server::LOW )
        return m_lowPriorityInst;
    else if( lvl == Server::MEDIUM )
        return m_mediumPriorityInst;
    else
        return m_highPriorityInst;
}

void Server::addPriorityInstrBlock( Server::PriorityLevel lvl, InstructionsBlock* inst )
{
    if( !inst )     // empty isntruction block. No use for it
        return;

    if( lvl == Server::LOW ) {
        if( !m_lowPriorityInst )
            m_lowPriorityInst = inst;
        else
            m_lowPriorityInst->addToTail( inst );
    }
    else if( lvl == Server::MEDIUM ) {
        if( !m_mediumPriorityInst )
            m_mediumPriorityInst = inst;
        else
            m_mediumPriorityInst->addToTail( inst );
    }
    else {          // high priorityInst
        if( !m_highPriorityInst )
            m_highPriorityInst = inst;
        else
            m_highPriorityInst->addToTail( inst );
    }
}

void Server::setPriorityInstrBlock( Server::PriorityLevel lvl, InstructionsBlock* inst )
{
    if( lvl == Server::LOW ) {
        if( m_lowPriorityInst == NULL )
            m_lowPriorityInst = inst;
        else {
            m_lowPriorityInst->addToTail( inst );            // append the new block on the tail
        }
    }
    else if( lvl == Server::MEDIUM ) {
        if( m_mediumPriorityInst == NULL )
            m_mediumPriorityInst = inst;
        else {
            m_mediumPriorityInst->addToTail( inst );
        }
    }
    else {
        if( m_highPriorityInst == NULL )
            m_highPriorityInst = inst;
        else {
            m_highPriorityInst->addToTail( inst );
        }
    }
}

Server::InstructionCounter* Server::instructionCounter() const
{
    return m_instructionCounter;
}

std::vector<std::string> Server::serverConfigs() const
{
    return m_configs;
}

std::string Server::config(int index) const
{
    if (index<0 || index >= m_configs.size()) return "";
    else return m_configs.at(index);
    
}

void Server::setServerConfigs( std::vector<std::string> list )
{
    m_configs = list;
}
std::vector<std::string> Server::serverMaps() const
{
    return m_maps;
}

std::string Server::map(int index) const
{
    if (index<0 || index >= m_maps.size()) return "";
    else return m_maps.at(index);
}

void Server::setServerMaps( std::vector<std::string> list )
{
    m_maps = list;
}

std::string Server::gameDirectory() const
{
    return m_gameDir;
}

void Server::setGameDirectory( std::string &dir )
{
    m_gameDir = dir;
}

int Server::commandPermission ( Commands cmd ) const
{
    return m_permissions[cmd];
}

void Server::setCommandPermission( Commands cmd, int value )
{
    m_permissions[cmd] = value;
}

Server::Warnings Server::warnings () const
{
    return m_warnings;
}

void Server::setWarnings ( Server::Warnings type )
{
    m_warnings = type;
}

Server::Timing Server::banNick () const
{
    return m_banNick;
}

void Server::setBanNick( Server::Timing time )
{
    m_banNick = time;
}

Server::Timing Server::banIp () const
{
    return m_banIp;
}

void Server::setBanIp( Server::Timing time )
{
    m_banIp = time;
}

bool Server::banWarnings () const
{
    return m_banWarnings;
}

void Server::setBanWarnings( bool option )
{
    m_banWarnings = option;
}
//**************************************** Functions for direct access to player vector ************************
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


//************************************* other ***********************

void Server::test_for_changes(Server* old)
{
  if ( old != NULL && old != 0)
  {
    if ( ( !old->m_valid ) || ( m_serverLog.compare(old->m_serverLog) != 0 ) || ( m_dbFolder.compare(old->m_dbFolder) != 0 ) )
    {
      //important changes. I'll delete old datas.
      m_changed = true;
    }
    else
    {
      //no important changes: i'll save old datas.
      //copy of players list
      for (unsigned int i=0; i<old->size(); i++ )
      {
        m_giocatori.push_back( (*old)[i]->clone() );
      }
      //copy of other params
      m_strict = old->m_strict;
      m_row = old->m_row;
      m_highPriorityInst = old->m_highPriorityInst;
      m_mediumPriorityInst = old->m_mediumPriorityInst;
      m_lowPriorityInst = old->m_lowPriorityInst;
      m_maps = old->m_maps;
      m_configs = old->m_configs;
    }
  }
  else
  {
    m_changed = true;
  }
}

bool Server::test_for_options()
{
  if ( !m_name.empty() && !m_rconpass.empty() && m_port!=0 && !m_backup.empty() && !m_botLog.empty() && !m_serverLog.empty() && !m_dbFolder.empty() && !m_gameDir.empty())
  {
      //i'll check for invalid command level and correct them
      for (unsigned int i=0; i < m_permissions.size(); i++)
      {
          if( m_permissions[i] < 0 ) m_permissions[i] = 0;
          else if( m_permissions[i] > 100 ) m_permissions[i] = 100;
      }
      return true;
  }
  return false;
}

bool Server::permitBalance(){
    time_t t;
    t = time( NULL );
    struct tm* timeInfo;
    timeInfo = localtime( &t );
    char buffer[80];    //to keep result of the time
    strftime( buffer, sizeof( buffer ), "%H:%M", timeInfo );
    std::string time( buffer );
    
    if (m_lastBalance.empty()){
        m_lastBalance = time;
        return true;
    }
    
    int hour = atoi(time.substr(0,time.find(':')).c_str());
    int minute = atoi(time.substr(time.find(':')+1).c_str());
    int l_hour = atoi(m_lastBalance.substr(0,m_lastBalance.find(':')).c_str());
    int l_minute = atoi(m_lastBalance.substr(m_lastBalance.find(':')+1).c_str());
    
    if ( (hour*60 + minute) - (l_hour*60 + l_minute) > 5 || (hour*60 + minute) - (l_hour*60 + l_minute) < 0 ){
        m_lastBalance = time;
        return true;
    }
    return false;
}

std::string Server::toString()
{
    std::string t;
    t.append( "{\n  Name : " );
    t.append( m_name );
    t.append( "\n  ConfigFile : " );
    t.append( m_configFile );
    t.append( "\n  Rcon : " );
    t.append( m_rconpass );
    t.append( "\n  Ip : " );
    t.append( m_ip );
    t.append( "\n  Port : " );
    t.append( handyFunctions::intToString( m_port ) );
    t.append( "\n  Backup dir : " );
    t.append( m_backup );
    t.append( "\n  Bot log : " );
    t.append( m_botLog );
    t.append( "\n  Server game log : ");
    t.append( m_serverLog );
    t.append( "\n  Database folder : ");
    t.append( m_dbFolder );
    t.append( "\n  Strict level : ");
    t.append( handyFunctions::intToString(m_strict) );
    t.append( "\n  Game folder : ");
    t.append( m_gameDir );
    t.append( "\n}\n" );
    return t;
}

#endif
