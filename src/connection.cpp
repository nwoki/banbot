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

Connection::Connection(vector<ConfigLoader::Option> opzioni): recvSize( 0 )
{
  for ( unsigned int i = 0; i < opzioni.size(); i++ ){
      if ( opzioni[i].name.compare( "IP" ) == 0 )
          ip.push_back( (char*)opzioni[i].value.c_str() );
      else if ( opzioni[i].name.compare( "PORT" ) == 0 )
        port.push_back( atoi( opzioni[i].value.c_str() ));
      else if ( opzioni[i].name.compare( "RCONPASS" ) == 0 )
          rconPass.push_back( opzioni[i].value);
  }
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
  //specials.push_back('\x02');
  for ( unsigned int i = 0; i < command.size(); i++ )
  {
    specials.push_back( command[i] );
  }
  return specials;
}

void Connection::prepareConnection( int server )
{
    socketID = socket( AF_INET, SOCK_DGRAM, 0 );

    serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons( port[server] );
    //recvSize = sizeof( serverAdd );

    hp = gethostbyname( ip[server] );
    memcpy( (char*)&serverAdd.sin_addr, (char*)hp->h_addr, hp->h_length );

    recvSize = sizeof( serverAdd );
}

void Connection::kick( string number, int server )
{
  prepareConnection( server );

  string comando( "rcon ");
  comando.append( rconPass[server] );
  comando.append( " kick " );
  comando.append( number );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close( socketID );
}

void Connection::say( string frase, int server )
{
  prepareConnection( server );

  string comando( "rcon " );
  comando.append( rconPass[server] );
  comando.append( " say \"" );
  comando.append( frase );
  comando.append( "\"" );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close( socketID );
}

void Connection::tell( string frase, string player, int server )
{
  prepareConnection( server );

  string comando( "rcon " );
  comando.append( rconPass[server] );
  comando.append( " tell " );
  comando.append( player );
  comando.append( " \"" );
  comando.append( frase );
  comando.append( "\"" );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
}

void Connection::reload(int server)
{
  if (server>=0)
  {
    string comando("rcon ");
    comando.append(rconPass[server]);
    comando.append(" reload");

    vector<char> command=makeCmd(comando);
    int bufferSize = command.size();
    prepareConnection(server);
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
    close(socketID);
  }
  else
  {
    for (int i=0;i<ip.size();i++)
    {
      string comando("rcon ");
      comando.append(rconPass[i]);
      comando.append(" reload");

      vector<char> command=makeCmd(comando);
      int bufferSize = command.size();
      prepareConnection(i);
      sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
      close(socketID);
      sleep(1);
    }
  }
}

void Connection::mute( string number, int server)
{
  prepareConnection( server );

  string comando( "rcon " );
  comando.append( rconPass[server] );
  comando.append( " mute " );
  comando.append( number );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
}
void Connection::unmute( string number, int server)
{
    prepareConnection( server );

  string comando( "rcon " );
  comando.append( rconPass[server] );
  comando.append( " unmute " );
  comando.append( number );

  vector< char > command = makeCmd( comando );
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
}
