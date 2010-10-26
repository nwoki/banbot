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

#include "handyFunctions.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <vector>

bool fileExistance( const std::string &pathToFile )
{
#ifdef DB_MODE
    std::cout << "FILEEXISTANCE -> path: " << pathToFile << std::endl;
#endif
    if( pathToFile.empty() ) {
        std::cout << "\e[0;31m ERROR: handyFunctions::checkExistance given empty path to check! \e[0m \n";
        return false;
    }

    struct stat st;

    if( stat( pathToFile.c_str(), &st ) != 0 )  //file doesn't exist
        return false;
    else    //exists
        return true;
}


bool dirCreate( std::string path )
{
#ifdef DB_MODE
    std::cout << "DIRCREATE " << path << std::endl;
#endif

    //check if the path is terminated, otherwise add slash
    if( path[path.size()] != '/' )
        path.append( "/" );

    int end = path.find_last_of( '/' );
    int pos = 0;
    bool ok = true;

    //check per la cartella
    while( pos < end && ok )
    {
        pos = path.find( '/', pos + 1 );
        std::string cartella = path.substr( 0, pos );

        struct stat st;
        if( stat( cartella.c_str(), &st ) != 0 )
        {
            if( mkdir( cartella.c_str(), 0777 ) != 0 )
            {
                std::cout<<"\e[1;31m[EPIC FAIL] couldn't create directory '"<<cartella<<"'.Please check permissions! \e[0m \n";
                ok = false;
            }
        }
    }
    return ok;
}

bool fileCreate( const std::string &file )
{

#ifdef DB_MODE
    std::cout << "fileCREATE " << file << std::endl;
#endif

    bool ok = true;
    //check per il file
    if( !file.empty() )
    {
        struct stat st;
        if( stat( file.c_str(), &st ) != 0 )
        {
            std::cout << "\e[0;33m[!] " << file << " doesn't exist...Creating it...\e[0m \n";
            //create logfile
            std::ofstream OUT( file.c_str() );

            if ( OUT.is_open() )
                OUT.close();
            else {
                std::cout << "\ee[1;31m[EPIC FAIL] couldn't create " << file << ". Please check permissions!\e[0m \n";
                return false;
            }
            return true;
        }
        else {
            std::cout << "\e[0;33mfile " << file << " already exists.. \e[0m \n";
            return false;
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


std::vector< std::string > detectRows ( std::string phrase)
{
    #define ROW 100         //max number of characters in a row
    std::vector< std::string > temp = std::vector< std::string >();
    unsigned int i=0;
    while ( i<phrase.size() )
    {
        unsigned int pos=phrase.find("\n",i);
        if ( (pos-i) > ROW )
        {
            temp.push_back( phrase.substr(i,ROW) );
            i+=ROW+1;
        }
        else
        {
            temp.push_back( phrase.substr(i,pos-i) );
            i=pos+1;
        }
    }
    return temp;
}