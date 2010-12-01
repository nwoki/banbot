/*
    FileLister.h is part of BanBot.

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

/*
 * Class used to list files in a given directory
 *
 */

#ifndef FILELISTER_H
#define FILELISTER_H

#include "ConfigLoader.h"
#include "string.h"

class FileLister
{

public:
    FileLister( ConfigLoader::Options* config );

private:
    std::vector< std::string > listFiles( const std::string &path );     // returns files in given path ( only files, all extensions! )

    ConfigLoader::Options* m_options;
};

#endif // FILELISTER_H
