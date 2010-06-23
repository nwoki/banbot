/*
 *
 *   	Analyzer.h is part of BanBot.
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
 *   	Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)
 *
 *
 *   	BanBot uses SQLite3:
 *   	Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
 *   	Software Foundation, Inc.
 *
 *
 *						Analyzer.
 *
 *	Author: 	[2s2h]Zamy
 *
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
 *	Description:	Fa da interfaccia ai file di log, si preoccupa di controllare
 *                 	gli utenti connessi ed i ban. Utilizza un'altra classe per accedere
 *                  	al database (per controllare gli utenti bannati) e per accedere
 *                  	al server (per kickare i giocatori o inviare messaggi).
*/

#ifndef _Analyzer_h_
#define _Analyzer_h_
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "connection.h"
#include "db.h"
#include "logger.h"
#include "ConfigLoader.h"
#include "Backup.h"

#define TIME_SLEEPING 3 //tempo di attesa tra un giro e l'altro nell'analisi dei log (più è basso, più alto sarà il consumo (e spreco) di risorse,
                        //d'altro canto più alto è, maggiore sarà il tempo di risposta del bot).
#define SOCKET_PAUSE 1  //per permettere al socket di funzionare bene, inserisco una pausa tra say e kick

//stringa stampata all'utente con i comandi disponibili (quando viene dato il comando !help).
#define COMMANDLIST "Comandi disponibili: !ban <number> [<reason>], !unban <id>, !kick <number>, !mute <number>, !find <nick>, !findop <nick>, !op <number>, !deop <id>, !strict <ON/OFF>."

class Analyzer
{
private:
    //regex
    std::ifstream *log;
    const char* CLIENT_CONNECT;
    const char* CLIENT_USER_INFO;
    const char* CLIENT_DISCONNECT;
    const char* BAN;
    const char* BAN_NUMBER;
    const char* GUID;
    const char* INITGAME;
    const char* FIND;
    const char* FINDOP;
    const char* UNBAN;
    const char* OP;
    const char* OP_NUMBER;
    const char* DEOP;
    const char* HELP;
    const char* KICK;
    const char* KICK_NUMBER;
    const char* SLAP;
    const char* SLAP_NUMBER;
    const char* SLAP_MORE;
    const char* MUTE;
    const char* MUTE_NUMBER;
    const char* STRICT;
    const char* COMMAND;
    const char* VETO;
    const char* STATUS;
    const char* FORCE;
    const char* FORCE_NUMBER;
    const char* IAMGOD;
    //variabili private
    std::vector<std::string> files;
    std::vector<std::string> BotLogFiles;
    std::vector<std::streampos> row;
    std::vector<bool> strict;
    Backup* backup;
    Logger* generalLog;
    //funzione per il caricamento delle opzioni
    void loadOptions(std::vector<ConfigLoader::Option> opzioni);
    //funzioni associate ai regex (comandi dal server/eventi)
    void clientUserInfo(char* line);
    void clientConnect(char* line);
    void clientDisconnect(char* line);
    void ban(char* line);
    void unban(char* line);
    void find(char* line);
    void findOp(char* line);
    void op(char* line);
    void deop(char* line);
    void help(char* line);
    void kick(char* line);
    void mute(char* line);
    void setStrict(char* line);
    void veto(char* line);
    void slap(char* line);
    void status(char* line);
    void force(char* line);
    void iamgod(char* line);
    //nuke

protected:
    bool isA(char* line,const std::string &regex);          //testa se la riga soddisfa il regex.
    bool isAdminSay(char *line, std::string &numero);		//ritorna true se la richiesta è stata fatta da un admin, salva in numero il numero del giocatore che ha fatto la richiesta.
    virtual void expansion(char* line);                     //slot per facilitare eventuali espansioni fatte da terze parti (basta ereditare da Analyzer e scrivere i metodi nuovi facendo un overloading di questo metodo)
    void getDateAndTime(std::string &date,std::string &time); //salva nelle stringhe data e ora.
    bool nickIsBanned(const std::string &nick);               //controlla se il nick è stato bannato.
    bool ipIsBanned(const std::string &ip);               //controlla se il nick è stato bannato.
    void buttaFuori(const std::vector< string >& reason, const std::string numero, const std::string nick);
    std::string correggi(std::string stringa);          //corregge la riga di testo per l'inserimento e l'utilizzo col database
    void tell(std::string frase, std::string player);  //invia la frase al giocatore usando connection::tell(), ma spezzettando automaticamente la stringa in più frasi se necessario.
    bool isStrict();                                    //ritorna true se il server e' in strict mode.
    std::vector<unsigned int> admins();                          //restituisce gli indici dell'array dei player dei giocatori attualmente nel server che sono admin.
    void tellToAdmins(std::string frase);               //invia un messaggio a tutti gli admin attualmente in game (messaggio privato).
    int translatePlayer(std::string player);            //@player rappresenta un nick o una parte del nick di un player in game, la funzione restituisce l'indice nell'array dei giocatori indicato da @player, se non trovato o non univoco restituisce -1.
    Logger* logger;
    Connection * server;
    Db * database;
    unsigned int serverNumber;
    class Player
    {
        public:
        std::string GUID;
        std::string number;
        std::string nick;
        std::string ip;
        Player():GUID(std::string("")),number(""),nick(""),ip(""){};
    };
    std::vector< std::vector<Player*> > giocatori;    //array di array (un array di player per server)

public:
    Analyzer( Connection*, Db*, Logger*, Backup*, vector< ConfigLoader::Option >);
    ~Analyzer();
    void main_loop();
};

#endif
