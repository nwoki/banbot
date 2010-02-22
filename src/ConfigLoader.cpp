/*		Implementazione di Config Loader
 *
 *	Author: [2s2h]Zamy
 *
 *	Note:	--
 *
*/

#ifndef _ConfigLoader_cpp_
#define _ConfigLoader_cpp_

#include "ConfigLoader.h"

//costruttore: apro il file
ConfigLoader::ConfigLoader(std::string filePath)
{
  cfg.open(filePath.c_str());
}
//distruttore:chiudo il file
ConfigLoader::~ConfigLoader()
{
  cfg.close();
}


std::vector<ConfigLoader::Option> ConfigLoader::getOptions()
{
  std::vector<ConfigLoader::Option> vettore;
  //leggo il file fino alla fine
  if (cfg.is_open())
  {
    while (!cfg.eof())
    {
      //leggo una riga
      char line [600];
      cfg.getline(line,300);
      //se non è un commento prendo le impostazioni
      if (line[0]!='#' && !cfg.eof())
      {
	ConfigLoader::Option opzione;
	
	//la trasformo in stringa e estraggo le coppie chiave-valore
	std::string riga=line;
	int end=riga.find("=");
	opzione.name=riga.substr(0,end);
	opzione.value=riga.substr(end+1,riga.size());
	
	//aggiungo al vettore
	vettore.push_back(opzione);
      }
    }
  }
  return vettore;
}

#endif