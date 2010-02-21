/*              Connection
 *
 *  Author:     [2s2h]n3m3s1s
 *
 *  Description:    classe utilizzata da Analyzer per dare i comandi al server
 *                  di gioco
*/


#include "connection.h"

Connection::Connection( string a, int b, string c ):  //ho solo bisogno di inizializzarmi i valori qui
    ip( (char*)a.c_str() ),
    port( b ),
    rconPass( c ),
    recvSize(0)
{
  
}

Connection::~Connection()
{

}

vector<char> Connection::makeCmd( string cmd) //cmd = "rcon " + pass + azione da compiere
{
  vector<char> command(cmd.begin(),cmd.end());
  vector<char> specials;
  for (int i=0;i<4;i++)
  {
    specials.push_back('\xff');
  }
  //specials.push_back('\x02');
  for (int i=0;i<command.size();i++)
  {
    specials.push_back(command[i]);
  }
  return specials;
}

void Connection::prepareConnection()
{
    socketID = socket( AF_INET, SOCK_DGRAM, 0 );

    serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons( port );
    recvSize = sizeof( serverAdd );
    
    hp = gethostbyname(ip);//127.0.0.1");"81.174.67.195"
    memcpy( (char*)&serverAdd.sin_addr, (char*)hp->h_addr, hp->h_length );
    
    recvSize = sizeof( serverAdd );
}

/*void Connection::ban(string number)   //recv number
{

}
*/
void Connection::kick(string number)
{
  prepareConnection();
  
  string comando("rcon ");
  comando.append(rconPass);
  comando.append(" kick ");
  comando.append(number);
  
  vector<char> command=makeCmd(comando);
  int bufferSize = command.size();
  sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );
  close(socketID);
}


/*void Connection::asd()
{

    prepareConnection();

    //sockaddr &serverAddCast = (sockaddr &)serverAdd;
    //sockaddr &clientAddCast = (sockaddr &)clientAdd;

    //int size = sizeof( serverAdd );
    recvSize = sizeof( serverAdd );

    vector<char> command=makeCmd("rcon asd status");
    cout<<"Result ";
    for(int i=0;i<command.size();i++)
    {
      cout<<command[i];
    }
    cout<<":\n";

    int bufferSize = command.size();


    sendto( socketID, command.data(), bufferSize, 0, &(sockaddr &)serverAdd, recvSize );

    close(socketID);
}*/

int main( int argc, char *argv[] )
{
  Connection *prova=new Connection("127.0.1.1",27960,"asd");
  prova->kick("2");
}