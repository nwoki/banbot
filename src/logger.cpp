/*              logger.
 *
 *  Author:         [2s2h]n3m3s1s
 *
 *  Description:    classe che gestisce il logfile del bot. Lo si inizializza
 *                  passandogli il percorso al file log. Se il file esiste usa
 *                  quello specificato, altrimenti se lo crea.
*/
#ifndef _logger_cpp_
#define _logger_cpp_

#include "logger.h"
#include <time.h>
#include <stdio.h>

Logger::Logger( std::string p ): path( p )
{
    //check per il file log
    std::cout<<"[-] checking for logfile..\n";
    std::ifstream IN( path.c_str() );
    if( IN.is_open() ) IN.close();
    else
    {
      std::cout<<"[!] Logfile doesn't exist..\n";
      //create logfile
      std::ofstream OUT(path.c_str(),std::ios_base::ate);
      if ( OUT.is_open() ) OUT.close();
      else
	std::cout<<"[ERR] couldn't create the log file.Please check permissions!\n";
    }
}

Logger::~Logger()
{
  file.close();
}

void Logger::changePath(std::string path)
{
  this->path=path;
}

bool Logger::open()
{
  file.open(path.c_str(),std::ios_base::ate);
  return file.is_open();
}

bool Logger::write( std::string text )
{
  if (file.is_open())
  {
    //stampo il timestamp e il messaggio
    time_t timestamp=time(NULL);
    file<<"["<<asctime(localtime(&timestamp))<<"] "<<text.append("\n");
    return true;
  }
  return false;
}

void Logger::close()
{
  file.close();
}

#endif