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

#define IP "127.0.0.1"  //server ip
#define PORT 27960  //server port
#define RCON "asd" //server rcon password
#define LOG "logs/bot.log"  //where logfile is located

int main( int argc, char *argv[] ){ //pass arguments to specify server logfile and bot logfile

    ConfigLoader * caricatore;

    if ( argc == 2 )
        caricatore = new ConfigLoader(argv[1]);
    else
        caricatore = new ConfigLoader( CONFIG );

    std::vector<ConfigLoader::Option> opzioni = caricatore->getOptions();

    delete caricatore;

    cout<<opzioni[0].name<< " " <<opzioni[0].value<<endl;
    cout<<opzioni[1].name<< " " <<opzioni[1].value<<endl;
    cout<<opzioni[2].name<< " " <<opzioni[2].value<<endl;



    Logger *logGay = new Logger( LOG );
    Db *d = new Db( logGay, opzioni) ;

    string a="127.0.0.1", b="asd";
    int c = 27960;

    Connection *serverCommand = new Connection( a,c,b);
    string file = "~/.q3a/q3ut4/game.log";
    Analyzer anal( serverCommand, d, file );
    anal.main_loop();


  //ho l'array con tutte le impostazioni

}