/*
 *
 *   	Backup.h is part of BanBot.
 *
 *   	BanBot is free software: you can redistribute it and/or modify
 *   	it under the terms of the GNU General Public License as published by
 *   	the Free Software Foundation, either version 3 of the License, or
 *   	(at your option) any later version.
 *
 *      BanBot is distributed in the hope that it will be useful,
 *   	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   	GNU General Public License for more details.
 *
 *   	You should have received a copy of the GNU General Public License
 *   	along with BanBot (look at GPL_License.txt).
 *   	If not, see <http://www.gnu.org/licenses/>.
 *
 *   	Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)
 *
 *
 *   	BanBot uses SQLite3:
 *   	Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
 *   	Software Foundation, Inc.
 *
 *					Backup
 *	Author:		[2s2h]Zamy
 *
 *	Description:	This class is to do a daily backup of all files (log, databases, etc.) at a specific time 
                    (default is 5:30 A.M.).
 *
 */
#ifndef _backup_h_
#define _backup_h_

#include <string>
#include <vector>

#include "ConfigLoader.h"
#include "logger.h"
#include "db.h"

//queste macro definiscono l'orario in cui effettuare l'archiviazione.
//se il bot non e' utilizzato in simil-realtime, per comodita' si puo' definire
//un arco di tempo in cui il lavoro deve venir svolto.
//per esempio, tra le 16:10 e le 16:32, diventa HOUR=15 MINUTE=10 OVERMINUTE=32
//se il bot viene impostato per fare il suo lavoro più volte al minuto,
//e' ragionevole impostare un solo minuto, e quindi con OVERMINUTE uguale a MINUTE.
//tanto per sicurezza, lascio uno spiraglio di 2 minuti:
#define HOUR "05"   //hour
#define MINUTE "30"  //minimum minute of the hour
#define OVERMINUTE "31" //maximum minute of the hour, with minimum defines the range within do the backup.

class Backup
{
  public:
    Backup( ConfigLoader::Options*, Db* );  //usa il file di log interno a ConfigLoader::Options.
    ~Backup();
    bool doJobs();                       //ritorna true se ha effettuato il backup
    void creaCartelle();                //crea le cartelle di backup dei server nuovi
  private:
    ConfigLoader::Options* m_options;
    Db* database;
    std::string nameOfDir();                     //calcola il nome della cartella
    void spostaFiles( unsigned int server, std::string dirOfTheDay ); //fa il backup di tutti i files del server @server.
    void spostaFilesGenerali( std::string dirOfTheDay );             //fa il backup dei file generali del bot (general log).
    void backupDatabases( std::string dirOfTheDay );                //backup dei database dei server.
    void checkFolder( std::string path );  //crea la cartella se non esiste.
    bool isTimeToWork();                //ritorna true se è il momento di fare il backup.
    bool done;
};
#endif