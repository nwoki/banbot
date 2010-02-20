#include <iostream>
#include <string>
#include <vector>
#include "db.h"
#include "connection.h"
#include "ConfigLoader.h"
#include "sqlite3.h"

#define CONFIG "BanBot.cfg"

int main( int argc, char *argv[] )
{
  ConfigLoader * caricatore;
  if (argc==2)
    caricatore=new ConfigLoader(argv[1]);
  else
    caricatore=new ConfigLoader(CONFIG);
  std::vector<ConfigLoader::Option> opzioni=caricatore->getOptions();
  delete caricatore;
  
  //ho l'array con tutte le impostazioni
  
}