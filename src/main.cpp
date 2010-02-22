#include <iostream>
#include <string>
#include <vector>
#include "Analyzer.h"
#include "db.h"
#include "connection.h"
#include "ConfigLoader.h"
#include "sqlite3/sqlite3.h"

#define CONFIG "BanBot.cfg"

using namespace std;


int main( int argc, char *argv[] ){ //pass arguments to specify server logfile and bot logfile

    ConfigLoader * caricatore;

    if ( argc == 2 )
        caricatore = new ConfigLoader(argv[1]);
    else
        caricatore = new ConfigLoader( CONFIG );

    std::vector<ConfigLoader::Option> opzioni = caricatore->getOptions();
    
    for (int i=0;i<opzioni.size();i++)
    {
      cout<<opzioni[i].name<<" = "<<opzioni[i].value<<"\n";
    }
    cout<<"\n";
  
    delete caricatore;

    Db *d = new Db(opzioni) ;

    int founded=0;
    string ip;
    int port=0;
    string password;
    string logpath;
    for (int i=0;founded<4 && i<opzioni.size();i++)
    {
      if (opzioni[i].name.compare("IP")==0)
      {
	ip=opzioni[i].value;
	founded++;
      }
      else if (opzioni[i].name.compare("PORT")==0)
      {
	port=atoi(opzioni[i].value.c_str());
	founded++;
      }
      else if (opzioni[i].name.compare("RCONPASS")==0)
      {
	password=opzioni[i].value;
	founded++;
      }
      else if (opzioni[i].name.compare("LOGPATH")==0)
      {
	logpath=opzioni[i].value;
	founded++;
      }
    }
    
    cout<<"[+] Importing values from config file:\n";
    cout<<"  [-] Ip of server: "<<ip<<"\n";
    cout<<"  [-] Port of server: "<<port<<"\n";
    cout<<"  [-] Rcon password: "<<password<<"\n";
    cout<<"  [-] File di log: "<<logpath<<"\n";

    Connection *serverCommand = new Connection(ip,port,password);

    Analyzer anal( serverCommand, d, logpath );
    anal.main_loop();


  //ho l'array con tutte le impostazioni

}
