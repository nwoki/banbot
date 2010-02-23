/*				Analyzer.
 *
 *	Author: 	[2s2h]Zamy
 *
<<<<<<< Updated upstream
 *	Description:	Fa da interfaccia ai file di log, si preoccupa di controllare gli utenti connessi ed i ban.
 *			Utilizza un'altra classe per accedere al database (per controllare gli utenti bannati) e per
 *			accedere al server (per kickare i giocatori).
 *
 *	Note:		Questa classe è progettata per essere facilmente espandibile nelle sue funzionalità.
 *			Per fare ciò, è sufficiente scrivere una classe che eredita da Analyzer, e ne sovrascrive
 *			il metodo virtuale "expansion(char* line)": infatti tutte le righe che non vengono "catturate"
 *			da Analyzer, vengono passate a questo metodo.
 *			Per effettuare test sulle righe per cercarne di particolari (per esempio un say che contiene
 *			la stringa "!slap") consiglio caldamente l'utilizzo del metodo isA(char* line,std::string regex),
 *			scritto apposta per eseguire questi test utilizzando i regex.
 *
=======
 *	Description:	Fa da interfaccia ai file di log, si preoccupa di controllare
 *                  gli utenti connessi ed i ban. Utilizza un'altra classe per accedere
 *                  al database (per controllare gli utenti bannati) e per accedere
 *                  al server (per kickare i giocatori).
>>>>>>> Stashed changes
*/

#ifndef _Analyzer_h_
#define _Analyzer_h_
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
/*
    Analyzer.h is part of BanBot.

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

#include "connection.h"
#include "db.h"
#include "logger.h"

class Analyzer
{
  private:
    class Player
    {
      public:
	std::string GUID;
	std::string number;
	Player():GUID(std::string("")),number(""){};
    };
    std::vector<Player*> giocatori;
    static const long TIME_SLEEPING = 5;
    std::ifstream log;
    const char* CLIENT_CONNECT;
    const char* CLIENT_USER_INFO;
    const char* CLIENT_DISCONNECT;
    const char* SAY;
    const char* GUID;
    const char* INITGAME;
    std::string file;
    std::streampos row;
    Connection * server;
    Db * database;
    Logger* logger;

  protected:
    bool isA(char* line,std::string regex);
    virtual void expansion(char* line);

  public:
    Analyzer( Connection*, Db*,Logger* ,std::string = NULL );
    ~Analyzer();
    void main_loop();
    void setFile(std::string);
};

#endif
