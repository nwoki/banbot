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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/

#ifndef HANDYFUNCTIONS_CPP
#define HANDYFUNCTIONS_CPP

#include "boost_1_45/boost/filesystem.hpp"
#include "handyFunctions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>
#include <regex.h>
#include <limits>

#define _R_STATUS_RESPONSE "^map:.*\n.*\n.*-{5}\n([ \t]*[0-9]+[ \t]+-{0,1}[0-9]+[ \t]+[0-9]+[ \t]+[^\n]+\\^7[ \t]+[0-9]+[ \t]+[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}:[0-9]+[ \t]+[0-9]+[ \t]+[0-9]+.*\n)+\n.*$"

namespace fs = boost::filesystem;

namespace handyFunctions{

    bool fileOrDirExistance( const std::string &pathToFile )
    {
    #ifdef DB_MODE
        std::cout << "FILEEXISTANCE -> path: " << pathToFile << std::endl;
    #endif
        if( pathToFile.empty() ) {
            std::cout << "\e[0;31m ERROR: handyFunctions::checkExistance given empty path to check! \e[0m \n";
            return false;
        }

        if( fs::exists( pathToFile ) )
            return true;
        else
            return false;
    }


    bool createDir( std::string path )
    {
    #ifdef DB_MODE
        std::cout << "DIRCREATE " << path << std::endl;
    #endif

        bool status = false;
        //check if the path is terminated, otherwise add slash
        if( path[path.size()] != '/' )
            path.append( "/" );

        if( !fs::exists( path ) ) {
            std::cout << "path doesn't exist, try and create\n";
            // create directories if more than one
            try {
                if( !fs::create_directories( path ) )
                    std::cout << "[ERROR] can't create directory\n" << std::endl;
                else
                    status = true;
            }
            catch( const std::exception &exception ) {
                std::cout << "\e[1;31m[ERROR]" << exception.what() << "\e[0m \n";
            }
        }
        return status;
    }


    bool createFile( const std::string &file )
    {

    #ifdef DB_MODE
        std::cout << "fileCREATE " << file << std::endl;
    #endif

        bool ok = true;
        //check per il file
        if( !file.empty() )
        {
            if( !fs::exists( file ) ) {
                std::cout << "\e[0;33m[!] " << file << " doesn't exist...Creating it...\e[0m \n";
                std::ofstream OUT( file.c_str() );

                if ( OUT.is_open() ) {
                    std::cout << "\e[0;32m[OK] Logfile " << file << " created!\e[0m \n";
                    OUT.close();
                }
                else {
                    std::cout << "\e[1;31m[EPIC FAIL] couldn't create " << file << ". Please check permissions!\e[0m \n";
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


    std::vector< std::string > split( const std::string &str, char ch )
    {
        std::vector< std::string > aux;                 //  vector with splitted strings to return
        std::string auxStr( str );                      //  use this copy so i can change it

        if( !str.length() )                             //  empty string
            return aux;

        int pos = 0;                                    //use this to move around

        while( pos != -1 ) {
            pos = auxStr.find( ch );                    //find occurance
            aux.push_back( auxStr.substr( 0, pos ) );   // add to vector
            auxStr.erase( 0, pos + 1 );                 // erase bit I don't need anymore
        }

        return aux;
    }

/**************************** THIS FUNCTION SUCKS! USE THE FUCKING ATOI instead of re-inventing (doing mistakes, too) the weel****************************************/
    int stringToInt( const std::string& str )
    {
        int num;
        std::istringstream sstrem( str );
        sstrem >> num;

        if( sstrem.good() )
            return num;
        else
            return -1;      // error
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


    void detectRows( std::string phrase, std::vector< std::string >* rows )
    {
        #define ROW 100         //max number of characters in a row
        rows->clear();
        unsigned int i=0;
        while ( i<phrase.size() )
        {
            unsigned int pos=phrase.find("\n",i);
            if ( (pos-i) > ROW )
            {
                rows->push_back( phrase.substr(i,ROW) );
                i+=ROW;
            }
            else
            {
                rows->push_back( phrase.substr(i,pos-i) );
                i=pos+1;
            }
        }
    }
    
    bool extractFromStatus( std::string msg, std::vector< stats >* datas ){
        if ( isA(msg.c_str(),_R_STATUS_RESPONSE) ){
            
            std::vector< std::string > rows;
            stringExplode(msg, "\n", &rows);
            
            for (unsigned int i=3; i<rows.size(); i++)
            {
                //i'm working on a player line.
                std::vector< std::string > infos;
                stringExplode(rows[i]," ",&infos);
                if (infos.size() < 8) return false;
                stats temp;
                temp.slot = infos[0];
                temp.score = atoi(infos[1].c_str());
                temp.ping = atoi(infos[2].c_str());
                temp.rate = infos[infos.size()-1];
                temp.qport = infos[infos.size()-2];
                std::vector<std::string> address;
                stringExplode(infos[infos.size()-3], ":", &address);
                if (address.size() < 2) return false;
                temp.ip = address[0];
                temp.port = address[1];
                std::string nick = infos[3];
                for (unsigned int i = 4; i < infos.size() - 3; i++){
                    nick += " ";
                    nick += infos[i];
                }
                unsigned int pos = nick.rfind("^7");
                if ( pos >= nick.size() ) return false;
                temp.nick = nick.substr(0,pos);
                #ifdef DEBUG_MODE
                std::cout<<"extracted:"<<temp.toString()<<"\n";
                #endif
                datas->push_back(temp);
            }
            return true;
        }
        return false;
    }
    
    bool isA(const char* line, const std::string& regex )
    {
        regex_t r;
        
        if ( regcomp( &r, regex.c_str(), REG_EXTENDED|REG_NOSUB ) == 0){
            int status = regexec( &r, line, ( size_t )0, NULL, 0 );
            regfree( &r );
            if( status == 0 )
                return true;
        }
        return false;
    }
    
    void stringExplode(std::string str, std::string separator, std::vector<std::string>* results){
        unsigned int found;
        found = str.find_first_of(separator);
        while(found < str.size()){
            if(found > 0){
                results->push_back(str.substr(0,found));
            }
            str = str.substr(found+1);
            found = str.find_first_of(separator);
        }
        if(str.length() > 0){
            results->push_back(str);
        }
    }
};

#endif  // HANDYFUNCTIONS_CPP
