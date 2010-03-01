/*				
 *
 *   	Backup.cpp is part of BanBot.
 *
 *   	BanBot is freesoftware: you can redistribute it and/or modify
 *   	it under the terms of the GNU General Public License as published by
 *   	the Free Software Foundation, either version 3 of the License, or
 *   	(at your option) any later version.
 *
 *	BanBot is distributed in the hope that it will be useful,
 *   	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   	GNU General Public License for more details.
 *
 *   	You should have received a copy of the GNU General Public License
 *   	along with BanBot (look at GPL_License.txt).
 *   	If not, see <http://www.gnu.org/licenses/>.
 *
 *   	Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)
 *
 *
 *   	BanBot uses SQLite3:
 *   	Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
 *   	Software Foundation, Inc.
 *
 */
#ifndef _backup_cpp_
#define _backup_cpp_
#include "Backup.h"

Backup::Backup(std::vector<ConfigLoader::Option> opzioni)
{
  for (unsigned int i=0;i<opzioni.size();i++)
  {
    if (opzioni[i].name.compare("LOGPATH")==0 || opzioni[i].name.compare("BOTLOG")==0 || opzioni[i].name.compare("BOTLOGPATH"))
    {
      files.push_back(opzioni[i].value);
    }
    else if (opzioni[i].name.compare("BACKUPPATH")==0)
    {
      directory=opzioni[i].value;
    }
  }
}

Backup::~Backup()
{
}

void Backup::doJobs()
{

}

#endif