#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

class Logger
{
    public:
        Logger( std::string );
        ~Logger();

        bool open();    //returns true if opened successfully
        void changePath(std::string path);
        void timestamp();   //writes a timestamp into file
        void close();
	
	void write(const char* valore); 
	void write(const int valore); 

    private:
        std::string path;    //location of logfile
        std::ofstream file;  //filestream

};

Logger& operator<<(Logger& l,const char* valore);
Logger& operator<<(Logger& l,const int valore);
#endif //LOGGER_H