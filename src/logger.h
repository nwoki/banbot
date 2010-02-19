#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;


class logger
{
    public:
        logger( string );
        ~logger();

        bool open();    //returns true if opened successfully
        bool write( string );   //returns true if written
        void close();

    private:
        string path;    //location of logfile

};






#endif //LOGGER_H