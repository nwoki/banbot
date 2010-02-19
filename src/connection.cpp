/*              Analyzer.
 *
 *  Author:     [2s2h]n3m3s1s
 *
 *  Description:    classe utilizzata da Analyzer per dare i comandi al server
 *                  di gioco
*/


#include "connection.h"

Connection::Connection( string a, int b, string c ):  //ho solo bisogno di inizializzarmi i valori qui
    ip( (char *)a.c_str() ),
    port( b ),
    rconPass( c )
{
    create();
}

Connection::~Connection()
{

}

int Connection::create()
{
    conn = socket( AF_INET, SOCK_DGRAM, 0 );

    if (conn < 0){
        cout<<"[ ERR connection::create ] can't create socket!\n";
        return conn;
    }

    cout<<ip<<endl;
    server.sin_family = AF_INET;
    hp = gethostbyname( ip );
    cout<<hp;

    if( hp == 0 ){
        cout<<"[ ERR connection::create ] can't find host\n";
        return 0;
    }

    //copies bytes
    bcopy( (char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons( port );

    length = sizeof( struct sockaddr_in );

    return 1;
}

void Connection::kick( string str )
{
    string command = "rcon kick ";
    command.append( str );

    cout<<command<<endl;


    //invio messaggio
    //int result = sendto( conn, buff, strlen( buff ), 0, server, length );

    //if ( result < 0 )
      //  cout<<"[ ERR connection::ban ] can't send to server!\n";

}
