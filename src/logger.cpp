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
      std::ofstream OUT(path.c_str());
      if ( OUT.is_open() ) OUT.close();
      else
	std::cout<<"[ERR] couldn't create the log file.Please check permissions!\n";
    }
}

Logger::~Logger()
{
  file.close();
}

void Logger::write(const char* valore)
{
  file<<valore;
}

void Logger::write(const int valore)
{
  file<<valore;
}

void Logger::changePath(std::string path)
{
  this->path=path;
}

bool Logger::open()
{
  file.open(path.c_str(),std::ios_base::app);
  return file.is_open();
}

void Logger::timestamp()
{
    //stampo il timestamp e il messaggio
    time_t timestamp=time(NULL);
    std::string data(asctime(localtime(&timestamp)));
    data=data.substr(0,data.size()-1);
    file<<"["<<data<<"] ";  //asctime(localtime(&timestamp))
}

void Logger::close()
{
  file.close();
}

//funzioni ESTERNE alla classe,di comodo
Logger& operator<<(Logger& l,const char* valore)
{
  l.write(valore);
  return l;
}
Logger& operator<<(Logger& l,const int valore)
{
  l.write(valore);
  return l;
} 
#endif
