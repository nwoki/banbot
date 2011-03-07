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

#include "boost_1_45/boost/filesystem.hpp"

#include "FileLister.h"

namespace bf = boost::filesystem;                   // just for ease of use

FileLister::FileLister( ConfigLoader::Options* config )
    : m_options( config )
{
    populateDefaultMaps();
}

void FileLister::updateServerConfigMapList()
{
    for( unsigned int i = 0; i < m_options->size(); i++ ) {
        std::vector< std::string > filesList = listFiles( (*m_options)[i].gameDirectory() );

        (*m_options)[i].setServerConfigs( configOrMapFiles( filesList, CONFIG ) );

        // create complete list of file. Default + custom maps
        std::vector< std::string >completeMapList = m_defaultMaps;
        std::vector< std::string >customMapList = configOrMapFiles( filesList, MAP );

        for( unsigned int j = 0; j < customMapList.size(); j++ )
            completeMapList.push_back( customMapList.at( j ) );

        #ifdef DEBUG_MODE
        for( unsigned int a = 0; a < completeMapList.size(); a++ )
            std::cout << "map: " << completeMapList.at( a ) << "\n";
        #endif

        // and finally set complete list of maps
        (*m_options)[i].setServerMaps( completeMapList );
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
            if( type == MAP ) {
                if( auxStr.find( "zpak" ) == std::string::npos )
                    auxList.push_back( auxStr.substr( 0, auxStr.length()-4 ) );         // add map to list eliminating extension
            }
            else
                auxList.push_back( auxStr );                                            // add config file to list
        }
    }

#ifdef DEBUG_MODE
    std::cout << "FileLister::configMapFiles files are: \n";
    for( unsigned int j = 0; j < auxList.size(); j++ )
        std::cout << auxList.at( j ) << "\n";
#endif
    return auxList;
}

std::vector< std::string > FileLister::listFiles( std::string path )
{
    std::vector< std::string >fileList;

    // check for dir existance
    if( !bf::exists( path ) ) {
        std::cout << "\e[1;31m[FAIL]FileLister::listFiles GameDir path: " << path << " doesn't exist!Please check your config! \e[0m \n ";
        return fileList;
    }

    if( path[path.size()-1] != '/' )
        path.append( "/" );

    // move to "q3ut4/" dir
    path.append( "q3ut4/" );

    // check for dir existance 2
    if( !bf::exists( path ) ) {
        std::cout << "\e[1;31m[FAIL]FileLister::listFiles 'q3ut4' folder doesn't exist in your GAMEDIR folder!Please check your config! \e[0m \n ";
        return fileList;
    }

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

void FileLister::populateDefaultMaps()
{
    m_defaultMaps.push_back( "ut4_casa" );
    m_defaultMaps.push_back( "ut4_kingdom" );
    m_defaultMaps.push_back( "ut4_turnpike" );
    m_defaultMaps.push_back( "ut4_abbey" );
    m_defaultMaps.push_back( "ut4_prague" );
    m_defaultMaps.push_back( "ut4_mandolin" );
    m_defaultMaps.push_back( "ut4_uptown" );
    m_defaultMaps.push_back( "ut4_algiers" );
    m_defaultMaps.push_back( "ut4_austria" );
    m_defaultMaps.push_back( "ut4_maya" );
    m_defaultMaps.push_back( "ut4_tombs" );
    m_defaultMaps.push_back( "ut4_elgin" );
    m_defaultMaps.push_back( "ut4_oildepot" );
    m_defaultMaps.push_back( "ut4_swim" );
    m_defaultMaps.push_back( "ut4_harbortown" );
    m_defaultMaps.push_back( "ut4_ramelle" );
    m_defaultMaps.push_back( "ut4_toxic" );
    m_defaultMaps.push_back( "ut4_sanc" );
    m_defaultMaps.push_back( "ut4_riyadh" );
    m_defaultMaps.push_back( "ut4_ambush" );
    m_defaultMaps.push_back( "ut4_eagle" );
    m_defaultMaps.push_back( "ut4_suburbs" );
    m_defaultMaps.push_back( "ut4_crossing" );
    m_defaultMaps.push_back( "ut4_subway" );
    m_defaultMaps.push_back( "ut4_tunis" );
    m_defaultMaps.push_back( "ut4_thingley" );
}
