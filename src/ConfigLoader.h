/*
    ConfigLoader.h is part of BanBot.

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

#ifndef _ConfigLoader_h_
#define _ConfigLoader_h_

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

class ConfigLoader
{
private:
    std::ifstream cfg;

public:
    ConfigLoader( const std::string &filePath );
    ~ConfigLoader();

    class Option    //use for adminlist as well
    {
    public:
        std::string name;
        std::string value;
    };

    class Banlist
    {
    public:
        std::string nick, ip, date, time, motive;
        std::vector< std::string > guids;
    };

    std::vector< ConfigLoader::Option > getOptions();
    std::vector< ConfigLoader::Banlist > getBanlist();
};
#endif
