/*
    connection.cpp is part of BanBot.

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

#ifndef CONNECTION_CPP
#define CONNECTION_CPP

#include "connection.h"

Connection::Connection(ConfigLoader::Options* opzioni)
  : recvSize( 0 )
  , m_options(opzioni)
{
  /*for ( unsigned int i = 0; i < opzioni.size(); i++ ){
      if ( opzioni[i].name.compare( "IP" ) == 0 )
          ip.push_back( (char*)opzioni[i].value.c_str() );
      else if ( opzioni[i].name.compare( "PORT" ) == 0 )
        port.push_back( atoi( opzioni[i].value.c_str() ));
      else if ( opzioni[i].name.compare( "RCONPASS" ) == 0 )
          rconPass.push_back( opzioni[i].value);
  }*/
}

Connection::~Connection()
{

}

std::vector< char > Connection::makeCmd( std::string cmd ) //cmd = "rcon " + pass + azione da compiere
{
  std::vector< char > command( cmd.begin(), cmd.end() );
  std::vector< char > specials;
  for ( int i = 0; i < 4; i++ )
  {
    specials.push_back( '\xff' );
  }
  for ( unsigned int i = 0; i < command.size(); i++ )
  {
    specials.push_back( command[i] );
  }
  return specials;
}

void Connection::prepareConnection(int server)
{
    socketID = socket( AF_INET, SOCK_DGRAM, 0 );

    serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons( (*m_options)[server].port() );
    //recvSize = sizeof( serverAdd );

    hp = gethostbyname( (*m_options)[server].ip().c_str() );
    memcpy( (char*)&serverAdd.sin_addr, (char*)hp->h_addr, hp->h_length );

    recvSize = sizeof( serverAdd );
}

void Connection::kick( std::string number, int server )
{
    prepareConnection( server );

    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " kick " );
    comando.append( number );

    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close( socketID );
    usleep(SOCKET_PAUSE);
}

void Connection::say( std::string phrase, int server )
{
    prepareConnection( server );

    std::string comando( "rcon " );
    comando.append( (*m_options)[server].rcon() );
    comando.append( " say \"" );
    comando.append( phrase );
    comando.append( "\"" );
    #ifdef DEBUG_MODE
    std::cout << "Sending command: " << comando << "\n";
    #endif
    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );

    usleep(SOCKET_PAUSE);
    close(socketID);
}

void Connection::bigtext( std::string phrase, int server )
{
    prepareConnection( server );

    std::string comando( "rcon " );
    comando.append( (*m_options)[server].rcon() );
    comando.append( " bigtext \"" );
    comando.append( phrase );
    comando.append( "\"");

    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::tell( std::string phrase, std::string player, int server )
{
    prepareConnection( server );

    std::string comando( "rcon " );
    comando.append( (*m_options)[server].rcon() );
    comando.append( " tell " );
    comando.append( player );
    comando.append( " \"" );
    comando.append( phrase );
    comando.append( "\"" );
    #ifdef DEBUG_MODE
        std::cout << "Sending command: " << comando << "\n";
    #endif
    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::reload( int server )
{
  if ( server >= 0 )
  {
    std::string comando("rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append(" reload");

    std::vector<char> command=makeCmd(comando);
    int bufferSize = command.size();
    prepareConnection( server );
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
  }
  else
  {
    for (unsigned int i=0;i<m_options->size();i++)
    {
      std::string comando("rcon ");
      comando.append( (*m_options)[i].rcon() );
      comando.append(" reload");

      std::vector<char> command=makeCmd(comando);
      int bufferSize = command.size();
      prepareConnection( i );
      sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
      close(socketID);

    }
  }
  usleep(SOCKET_PAUSE);
}

void Connection::mute( std::string number, int server )
{
  prepareConnection( server );

  std::string comando( "rcon " );
  comando.append( (*m_options)[server].rcon() );
  comando.append( " mute " );
  comando.append( number );

  std::vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::muteAll( std::string admin, int server )
{
    prepareConnection( server );

    for ( int unsigned i=0; i<(*m_options)[server].size(); i++)
    {
        if ( (*m_options)[server][i]->number.compare( admin ) != 0 )
        {
            std::string comando( "rcon " );
            comando.append( (*m_options)[server].rcon() );
            comando.append( " mute " );
            comando.append( (*m_options)[server][i]->number );

            std::vector< char > command = makeCmd( comando );
            int bufferSize = command.size();
            sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
            usleep(SOCKET_PAUSE);
        }
    }
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::veto( int server )
{
  prepareConnection( server );

  std::string comando( "rcon " );
  comando.append( (*m_options)[server].rcon() );
  comando.append( " veto" );

  std::vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::slap( std::string number, int server )
{
  prepareConnection( server );

  std::string comando( "rcon ");
  comando.append( (*m_options)[server].rcon() );
  comando.append( " slap " );
  comando.append( number );

  std::vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::nuke( std::string number, int server )
{
  prepareConnection( server );

  std::string comando( "rcon ");
  comando.append( (*m_options)[server].rcon() );
  comando.append( " nuke " );
  comando.append( number );

  std::vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::force( std::string number, std::string where, int server )
{
  prepareConnection( server );

  std::string comando( "rcon " );
  comando.append( (*m_options)[server].rcon() );
  comando.append( " forceteam " );
  comando.append(number);
  comando.append(" ");
  comando.append(where);

  std::vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::map( std::string name, int server )
{
    prepareConnection( server );

    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " map " );
    comando.append( name );

    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::nextmap( std::string name, int server )
{
    prepareConnection( server );

    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " g_nextmap " );
    comando.append( name );

    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::changePassword( std::string pass, int server )
{
    prepareConnection( server );

    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " g_password " );
    comando.append( pass );

    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::exec( std::string file, int server )
{
    prepareConnection( server );

    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " exec " );
    comando.append( file );

    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::restart( int server )
{
    prepareConnection( server );
    
    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " restart" );
    
    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

#endif  // CONNECTION_CPP