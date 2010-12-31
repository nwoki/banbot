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

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "FileLister.h"

namespace bf = boost::filesystem;                   // just for ease of use

FileLister::FileLister( ConfigLoader::Options* config )
    : m_options( config )
{
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

    return fileList;
}


