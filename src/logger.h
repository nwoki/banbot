/*
    logger.h is part of BanBot.

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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/


#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

class Logger
{
    public:
        Logger( std::string );
        Logger();
        ~Logger();

        bool open();    //returns true if opened successfully. This function is not necessary, file is opened automatically on first write.
        void changePath(std::string path);
        void timestamp();   //writes a timestamp into file
        void close();	//close the file

        void write(const char* valore); //writes data on the file (but it's better to use the <<).
        void write(const int valore);

    private:
        std::string path;    //location of logfile
        std::ofstream file;  //filestream
        bool isOpen;
        void checkFile();   //check if directory and file exist, oterwise try to create it.

};

Logger& operator<<(Logger& l,const char* valore);
Logger& operator<<(Logger& l, std::string valore);
Logger& operator<<(Logger& l,const int valore);
#endif //LOGGER_H