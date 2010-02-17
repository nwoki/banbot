#include "connection.h"

connection::connection( string a, int b, string c ):  //ho solo bisogno di inizializzarmi i valori qui
    ip( (char *)a.c_str() ), 
    port( b ),
    rconPass( c )
{
    create();
}

connection::~connection()
{
    
}

int connection::create()
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

int connection::ban( string str )
{
    string command = "rcon ban ";
    command.append( str );
    
    cout<<command<<endl;

    
    //invio messaggio
    int result = sendto( conn, buff, strlen( buff ), 0, server, length );
    
    if ( result < 0 )
        cout<<"[ ERR connection::ban ] can't send to server!\n";
    
}

/*void connection::kick( string num )
{

}*/
