/*
    ConfigLoader.cpp is part of BanBot.

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

#ifndef _ConfigLoader_cpp_
#define _ConfigLoader_cpp_

#include "ConfigLoader.h"

//costruttore: apro il file
ConfigLoader::ConfigLoader( const std::string &filePath)
{
  cfg.open( filePath.c_str() );
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

std::vector<ConfigLoader::Banlist> ConfigLoader::getBanlist()
{
    std::vector<ConfigLoader::Banlist> vettore;

    if( cfg.is_open() ){
        while( !cfg.eof() ){
            //leggo righe
            char line[600];
            cfg.getline( line, 300 );

            ConfigLoader::Banlist banlist;

            //load banlist gen info ( nick, ip, date,ecc )

            while( line[0] != '=' && !cfg.eof() ){
                std::string riga = line;    //la trasformo in stringa e estraggo le guid
                int end = riga.find( "=" );

                //DEBUG
                #ifdef DEBUG_MODE
                    std::cout<< "\e[1;31m readline -> \e[0;m" << line << std::endl;
                #endif
                if( riga.substr( 0, end ) == "nick" )
                    banlist.nick = riga.substr( end + 1, riga.size() );

                else if ( riga.substr( 0, end ) == "ip" )
                     banlist.ip = riga.substr( end + 1, riga.size() );

                else if ( riga.substr( 0, end ) == "date" )
                     banlist.date = riga.substr( end + 1, riga.size() );

                else if ( riga.substr( 0, end ) == "time" )
                     banlist.time = riga.substr( end + 1, riga.size() );

                else if ( riga.substr( 0, end ) == "motive" )
                     banlist.motive = riga.substr( end + 1, riga.size() );

                else if ( riga.substr( 0, end ) == "author" )
                    banlist.author = riga.substr( end + 1, riga.size() );

                cfg.getline( line, 300 );   //get new line
            }

            while( line[0] != '#' && !cfg.eof() ){
                std::string riga = line;
                int end = riga.find( "=" );

                //DEBUG
                #ifdef DEBUG_MODE
                    std::cout<< "\e[1;31m readline -> \e[0;m" << riga << std::endl;
                #endif

                if( ( riga.substr( 0, end ) == "GUID" ) )
                    banlist.guids.push_back( riga.substr( end + 1, riga.size() ) );

                cfg.getline( line, 300 );
            }
            vettore.push_back( banlist );
        }
        //vettore.push_back( banlist );
    }
    return vettore;
}


#endif