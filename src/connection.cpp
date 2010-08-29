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

    Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/


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

vector< char > Connection::makeCmd( string cmd ) //cmd = "rcon " + pass + azione da compiere
{
  vector< char > command( cmd.begin(), cmd.end() );
  vector< char > specials;
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

void Connection::kick( string number )
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon ");
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " kick " );
  comando.append( number );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close( socketID );
  usleep(SOCKET_PAUSE);
}

void Connection::say( string frase )
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon " );
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " say \"" );
  unsigned int i=0;
  while ( i<frase.size() )
  {
      unsigned int pos=frase.find("\n",i);
      string finale ( comando );
      if ( (pos-i) > ROW )
      {
          finale.append( frase.substr(i,ROW) );
          i+=ROW+1;
      }
      else
      {
          finale.append( frase.substr(i,pos-i) );
          i=pos+1;
      }
      finale.append( "\"" );
      #ifdef DEBUG_MODE
      std::cout << "Sending command: " << finale << "\n";
      #endif
      vector< char > command = makeCmd( finale );
      int bufferSize = command.size();
      sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
      usleep(SOCKET_PAUSE);
  }
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::bigtext( string frase )
{
    prepareConnection( m_options->serverNumber );
    
    string comando( "rcon " );
    comando.append( (*m_options)[m_options->serverNumber].rcon() );
    comando.append( " bigtext \"" );
    comando.append( frase );
    comando.append( "\"");
    
    vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::tell( string frase, string player )
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon " );
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " tell " );
  comando.append( player );
  comando.append( " \"" );
  unsigned int i=0;
  while ( i<frase.size() )
  {
    unsigned int pos=frase.find("\n",i);
    string finale ( comando );
    if ( (pos-i) > ROW )
    {
      finale.append( frase.substr(i,ROW) );
      i+=ROW+1;
    }
    else
    {
      finale.append( frase.substr(i,pos-i) );
      i=pos+1;
    }
    finale.append( "\"" );
    #ifdef DEBUG_MODE
        std::cout << "Sending command: " << finale << "\n";
    #endif
    vector< char > command = makeCmd( finale );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    usleep(SOCKET_PAUSE);
  }
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::reload(bool all)
{
  if (!all)
  {
    string comando("rcon ");
    comando.append( (*m_options)[m_options->serverNumber].rcon() );
    comando.append(" reload");

    vector<char> command=makeCmd(comando);
    int bufferSize = command.size();
    prepareConnection( m_options->serverNumber );
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
  }
  else
  {
    for (unsigned int i=0;i<m_options->size();i++)
    {
      string comando("rcon ");
      comando.append( (*m_options)[i].rcon() );
      comando.append(" reload");

      vector<char> command=makeCmd(comando);
      int bufferSize = command.size();
      prepareConnection( i );
      sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
      close(socketID);
      usleep(SOCKET_PAUSE);
    }
  }
}

void Connection::mute( string number)
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon " );
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " mute " );
  comando.append( number );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::muteAll( string admin )
{
    prepareConnection( m_options->serverNumber );
    
    for ( int unsigned i=0; i<(*m_options)[m_options->serverNumber].size(); i++)
    {
        if ( (*m_options)[m_options->serverNumber][i]->number.compare( admin ) != 0 )
        {
            string comando( "rcon " );
            comando.append( (*m_options)[m_options->serverNumber].rcon() );
            comando.append( " mute " );
            comando.append( (*m_options)[m_options->serverNumber][i]->number );
            
            vector< char > command = makeCmd( comando );
            int bufferSize = command.size();
            sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
            usleep(SOCKET_PAUSE);
        }
    }
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::veto()
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon " );
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " veto" );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::slap( string number )
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon ");
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " slap " );
  comando.append( number );
  
  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::nuke( string number )
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon ");
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " nuke " );
  comando.append( number );
  
  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::force( string number, string where)
{
  prepareConnection( m_options->serverNumber );

  string comando( "rcon " );
  comando.append( (*m_options)[m_options->serverNumber].rcon() );
  comando.append( " forceteam " );
  comando.append(number);
  comando.append(" ");
  comando.append(where);

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
  usleep(SOCKET_PAUSE);
}

void Connection::map( string name )
{
    prepareConnection( m_options->serverNumber );
    
    string comando( "rcon ");
    comando.append( (*m_options)[m_options->serverNumber].rcon() );
    comando.append( " map " );
    comando.append( name );
    
    vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}

void Connection::nextmap( string name )
{
    prepareConnection( m_options->serverNumber );
    
    string comando( "rcon ");
    comando.append( (*m_options)[m_options->serverNumber].rcon() );
    comando.append( " nextmap " );
    comando.append( name );
    
    vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
    usleep(SOCKET_PAUSE);
}