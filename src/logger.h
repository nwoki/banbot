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
        bool write( std::string );   //returns true if written
        void close();

    private:
        std::string path;    //location of logfile
	std::ofstream file;  //filestream

};
#endif //LOGGER_H