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
#include <sys/stat.h>
#include <stdlib.h>

Logger::Logger( std::string p ): path( p )
{
    size_t pos=path.find_last_of('/');
    std::string cartella=path.substr(0,pos);
    //std::string file=path.substr(pos+1);
    
    //check per la cartella
    struct stat st;
    if( stat( cartella.c_str(), &st ) == 0 )
    {
        std::cout<<"  [*]dir '"<<cartella<<"/' found\n";
    }
    else
    {
        std::cout<<"  [!]couldn't find dir '"<<cartella<<"/'! Creating dir '"<<cartella<<"/'..\n";

	std::string command("mkdir \"");
	command.append(cartella);
	command.append("\"");
        if( !system(command.c_str()))
	{
            std::cout<<"  [OK]created '"<<cartella<<"/' directory..\n";
	}
        else
	{
            std::cout<<"[EPIC FAIL] couldn't create directory '"<<cartella<<"/'.Please check permissions!\n";
	}
    }
    //check per il file log
    std::cout<<"[-] checking for logfile..\n";
    std::ifstream IN( path.c_str() );
    if( IN.is_open() ) IN.close();
    else
    {
      std::cout<<"[!] Logfile doesn't exist... I'll create it.\n";
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
    file<<"\n["<<data<<"] ";  //asctime(localtime(&timestamp))
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
Logger& operator<<(Logger& l,std::string valore)
{
  l.write(valore.c_str());
  return l;
}
Logger& operator<<(Logger& l,const int valore)
{
  l.write(valore);
  return l;
} 
#endif
