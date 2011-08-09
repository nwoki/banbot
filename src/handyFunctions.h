/*
    handyFunctions.h is part of BanBot.

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

#ifndef HANDYFUNCTIONS_H
#define HANDYFUNCTIONS_H

#include <sstream>
#include <string>
#include <vector>

namespace handyFunctions {

    bool fileOrDirExistance( const std::string &pathToFileOrDir );  // checks if given directory or file exists
    bool createDir( std::string path );                             // creates given directory
    bool createFile( const std::string &file );                     // checks for existance of given file otherwise tries to create it
    std::string intToString( int number );                          // converts int to string
    std::vector< std::string >split( std::string str, char ch );    // splits given string every occurance of "ch"
    int stringToInt( const std::string &str );                      // transforms given string to int -1 on error
    std::string timeStamp();                                        // returns timestamp of current localtime  TODO various ways, date, time long date ecc
    void detectRows ( std::string, std::vector< std::string >* );   // split the string in rows, depending on /n and max characters per row (100).
    
    struct stats {
        std::string slot;
        int score;
        int ping;
        std::string nick;
        std::string ip;
        std::string port;
        std::string qport;
        std::string rate;
        std::string toString(){
            std::string t = slot;
            t+= " ";
            t+= handyFunctions::intToString(score);
            t+= " ";
            t+= handyFunctions::intToString(ping);
            t+= " ";
            t+= nick;
            t+= " ";
            t+= ip;
            t+= ":";
            t+= port;
            t+= " ";
            t+= qport;
            t+= " ";
            t+= rate;
            return t;
        }
    };
    
    bool extractFromStatus( std::string, std::vector< stats >* );   // extract all the info of the status command, dividing it into a struct. Returns true if success, else return false.
    bool isA(const char* line, const std::string& regex );          // exec a regex and check if the @line respect the rule of @regex
    void stringExplode(std::string str, std::string separator, std::vector<std::string>* results); // explode the string in a vector, basing on separator.
    std::string correggi(std::string stringa);                      // correct a string to be compatible with the database
};

#endif  //HANDYFUNCTIONS_H