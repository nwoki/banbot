/*
handyFunctions.cpp is part of BanBot.

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

//#include "handyFunctions.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <vector>


bool checkDirAndCreate( std::string path )
{
    int end = path.find_last_of( '/' ); //linux slash for paths
    int pos = 0;
    bool ok = true;

    //check for folder
    while( pos < end && ok )
    {
        pos = path.find( '/', pos + 1 );
        std::string cartella = path.substr( 0, pos );

        struct stat st;
        if( stat( cartella.c_str(), &st ) != 0 )
        {
            if( mkdir( cartella.c_str(), 0777 ) != 0 )
            {
                std::cout << "[EPIC FAIL] couldn't create directory/folder '" << cartella << "/'.Please check permissions!\n";
                ok = false;
            }
        }
        else {
            std::cout << "DIRECTORY " << cartella << "EXISTS :D\n";
        }
    }
    return ok;
}

std::string intToString( int number )
{
    std::stringstream out;  //throw it out on cout...
    out << number;  //..and catch it immediatly
    return out.str();
}


std::vector< std::string >split( const std::string &str, char ch )
{
    std::vector< std::string > aux; //  vector with splitted strings to return
    std::string auxStr( str );   //  use this copy so i can change it

    if( !str.length() ) //  empty string
        return aux;

    int pos = 0;    //use this to move around

    while( pos != -1 ) {
        pos = auxStr.find( ch );    //find occurance
        aux.push_back( auxStr.substr( 0, pos ) );   // add to vector
        auxStr.erase( 0, pos + 1 ); // erase bit I don't need anymore
    }

    return aux;
}


std::string timeStamp()
{
    time_t t;
    t = time( NULL );
    struct tm* timeInfo;
    timeInfo = localtime( &t );
    char buffer[80];    //to keep result of the time
    strftime( buffer, sizeof( buffer ), "%d-%m-%Y", timeInfo );
    return std::string( buffer );
}

// int main()
// {
//     std::cout << timeStamp();
// }


