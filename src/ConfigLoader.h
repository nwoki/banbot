/*				Config Loader
 *
 *	Author: 	[2s2h]Zamy
 *
 *	Description:	Carica le impostazioni dal file .config.
 *			La si istanzia passandogli il percorso al file, si richiama il metodo getOptions()
 *			per ottenere le impostazioni, e si distrugge l'oggetto per chiudere il file.
 *
*/

#ifndef _ConfigLoader_h_
#define _ConfigLoader_h_

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

class ConfigLoader
{
  private:
    std::ifstream cfg;
  public:
    ConfigLoader( std::string filePath );
    ~ConfigLoader();
    class Option
    {
      public:
	std::string name;
	std::string value;
    };
    std::vector<ConfigLoader::Option> getOptions();
};
#endif
