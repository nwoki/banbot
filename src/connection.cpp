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
#include <fcntl.h>

#define PORT 6873
#define HOST "service.2s2h.com"

Connection::Connection(ConfigLoader::Options* opzioni)
  : m_options(opzioni)
{
    socketID = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    bzero( &serverAdd, sizeof (serverAdd) );
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons( (*m_options).generalPort );
    serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /*
    //force the receive buffer size
    int n = 1024 * 1024;
    if (setsockopt(socketID, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
        // deal with failure, or ignore if you can live with the default size
        std::cout<<"\nSocket warning: unable to change the buffer size.\n";
        perror("");
        std::cout<<"\n";
    }*/
    
    
    if ( bind( socketID, (sockaddr *) &serverAdd, sizeof serverAdd) < 0 ){
        std::cout<<"\nSocket error: unable to open it.\n";
        perror("");
        std::cout<<"\n";
        exit(-1);
    }
}

Connection::~Connection()
{
    close(socketID);
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
    bzero( &serverAdd, sizeof (serverAdd) );
    serverAdd.sin_family = AF_INET;
    struct hostent *hp;
    if ( server >= 0){
        serverAdd.sin_port = htons( (*m_options)[server].port() );
        hp = gethostbyname( (*m_options)[server].ip().c_str() );
    }
    else{
        serverAdd.sin_port = htons( PORT );
        hp = gethostbyname( HOST );
    }
    memcpy( (char*)&serverAdd.sin_addr, (char*)hp->h_addr, hp->h_length );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
        sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
            sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
        }
    }
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
            sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
            usleep(SOCKET_PAUSE);
        }
    }
}

void Connection::veto( int server )
{
    prepareConnection( server );
    
    std::string comando( "rcon " );
    comando.append( (*m_options)[server].rcon() );
    comando.append( " veto" );
    
    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
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
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
}

void Connection::restart( int server )
{
    prepareConnection( server );
    
    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " restart" );
    
    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
}

std::string Connection::status( int server )
{
    prepareConnection( server );
    std::string comando( "rcon " );
    comando.append( (*m_options)[server].rcon() );
    comando.append( " status" );
    
    std::vector< char > command = makeCmd( comando );
    
    #ifdef DEBUG_MODE
    std::cout<<">>"<<comando<<"\n";
    #endif
    
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
    Packets * container = new Packets();
    receive(container);
    std::string final = container->rebuild();
    delete container;
    return final;
}
void Connection::gravity( std::string amount, int server )
{
    prepareConnection( server );
    
    std::string comando( "rcon ");
    comando.append( (*m_options)[server].rcon() );
    comando.append( " g_gravity " );
    comando.append( amount );
    
    std::vector< char > command = makeCmd( comando );
    int bufferSize = command.size();
    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
}

void Connection::sendInfo()
{
    prepareConnection(-1);
    for (unsigned int i = 0; i<(*m_options).size(); i++){
        std::string cmd ("<BanBot><serverName>");
        cmd.append((*m_options)[i].name());
        cmd.append("</serverName><port>");
        cmd.append(handyFunctions::intToString((*m_options)[i].port()));
        cmd.append("</port><version>");
        cmd.append(_VERSION);
        cmd.append("</version></BanBot>");
        std::cout<<"Sending : "<<cmd<<"\n";
        sendto( socketID, cmd.c_str(), cmd.size(), 0, &(sockaddr &)serverAdd, sizeof( serverAdd ) );
    }
}

int Connection::wait_packets(long sec, long usec)
{
    fd_set mask;
    struct timeval tv;
    
    FD_ZERO(&mask);
    FD_SET(socketID, &mask);
    
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    
    return select(socketID+1, &mask, NULL, NULL, &tv);
}

void Connection::receive(Packets * container){
    //set the socket as nonblocking
    //int flags = fcntl(socketID, F_GETFL);
    //fcntl(socketID, F_SETFL, flags | O_NONBLOCK);
    int t1 = 1;
    int t2 = 0;
    while ( wait_packets(t1,t2) > 0 ){
        #ifdef DEBUG_MODE
        std::cout<<"Receiving a packet!\n";
        #endif
        t1=0;
        t2=100000;
        char buf [2000];
        socklen_t adrSize = sizeof(serverAdd);
        int rec = recvfrom( socketID, buf, sizeof buf, 0, &(sockaddr &)serverAdd, &adrSize );
        if (rec > 0){
            container->addPacket(std::string (buf).substr(0,rec));
            #ifdef DEBUG_MODE
            std::cout<<"Received:\n"<<buf<<"stop!\n";
            #endif
        }
    }
    //return to blocking
    //fcntl(socketID, F_SETFL, flags);
    #ifdef DEBUG_MODE
        std::cout<<"Result:\n"<<container->rebuild()<<"stop!\n";
        (*m_options->errors)<<"Result:\n"<<container->rebuild()<<"stop!\n";
    #endif
}
#endif  // CONNECTION_CPP