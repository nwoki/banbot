/*
    FileLister.cpp is part of BanBot.

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
#ifndef FILELISTER_CPP
#define FILELISTER_CPP

#include "boost_1_45/boost/filesystem.hpp"

#include "FileLister.h"

namespace bf = boost::filesystem;                   // just for ease of use

FileLister::FileLister( ConfigLoader::Options* config )
    : m_options( config )
{
}

void FileLister::updateServerConfigMapList()
{
    for( unsigned int i = 0; i < m_options->size(); i++ ) {
        std::vector< std::string > filesList = listFiles( (*m_options)[i].gameDirectory() );
        (*m_options)[i].setServerConfigs( configOrMapFiles( filesList, CONFIG ) );
        (*m_options)[i].setServerMaps( configOrMapFiles( filesList, MAP ) );
    }
}


/*********************
 * PRIVATE FUNCTIONS *
 ********************/

std::vector< std::string > FileLister::configOrMapFiles( std::vector< std::string > filesList, fileType type )
{
    std::string suffix;

    if( type == CONFIG )
        suffix = ".cfg";
    else
        suffix = ".pk3";

    std::vector< std::string > auxList;

    for( unsigned int i = 0; i < filesList.size(); i++ ) {
        std::string auxStr = filesList.at( i );
        int strSize = auxStr.size();

        if( auxStr.substr( strSize - 4 ).compare( suffix ) == 0 ) {
            #ifdef DEBUG_MODE
            std::cout << "File: " << auxStr << "\n";
            std::cout << "ending is: " << auxStr.substr( strSize - 4 ) << "\n";
            #endif
            auxList.push_back( auxStr );        // add to list
        }
    }

#ifdef DEBUG_MODE
    std::cout << "FileLister::configMapFiles files are: \n";
    for( unsigned int j = 0; j < auxList.size(); j++ )
        std::cout << auxList.at( j ) << "\n";
#endif
    return auxList;
}

std::vector< std::string > FileLister::listFiles( const std::string& path )
{
    std::vector< std::string >fileList;
    bf::path dirToScan( path );
    bf::basic_directory_iterator< bf::path >endIt;  // automatically set to end of dir

    for( bf::basic_directory_iterator< bf::path >it( dirToScan ); it != endIt; it++ ) {
        if( !bf::is_directory( *it ) ) {
            bf::path auxFile( *it );
            fileList.push_back( auxFile.filename() );
        }
    }

#ifdef DEBUG_MODE
    std::cout << "FileLister::listFiles files are: \n";
    for( unsigned int i = 0; i < fileList.size(); i++ )
        std::cout << fileList.at( i ) << "\n";
#endif

    return fileList;
}

#endif  // FILELISTER_CPP


