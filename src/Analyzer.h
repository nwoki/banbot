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
 *   	Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)
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
 *	Note:	
 *
 *	Description:	Parte principale del bot, si preoccupa di controllare
 *                 	gli utenti connessi ed i ban. Utilizza un'altra classe per accedere
 *                  al database (per controllare gli utenti bannati) e per accedere
 *                  al server (per kickare i giocatori o inviare messaggi).
*/

#ifndef _Analyzer_h_
#define _Analyzer_h_

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "db.h"
#include "logger.h"
#include "ConfigLoader.h"
#include "Backup.h"
#include "Scheduler.h"
#include "FileLister.h"

#define _VERSION "1.2b1"

//tempi di attesa tra un giro e l'altro nell'analisi dei log (più è basso, più alto sarà il consumo (e spreco) di risorse,
//d'altro canto più alto è, maggiore sarà il tempo di risposta del bot).
#define TIME_SLEEPING_MIN 1         //minimum pause time (when events/commands occurs), aka fascia 0
#define TIME_SLEEPING_MIDDLE 3      //middle pause time (after approx 30 seconds of inactivity), aka fascia 1
#define TIME_SLEEPING_MAX 15        //maximum pause time, aka stand-by mode, (after approx 3 minutes if inactivity in fascia 1), aka fascia 2

//strict levels:
#define LEVEL0 0        // Bot deactivated. Reacts only to strict and status commands.
#define LEVEL1 1        // no anti-cheat controls activated, only banned players will be kicked.
#define LEVEL2 2        // anticheat checks, only warnings.
#define LEVEL3 3        // automated ban, warning if in dubt.
#define LEVEL4 4        // like level 3, automated ban if in dubt too.

#define ADVANCED_LEVEL0 0 // no advanced checks.
#define ADVANCED_LEVEL1 1 // advanced checks, only warnings.
#define ADVANCED_LEVEL2 2 // advanced checks, ban automatically.

//group of not-opped players
#define H_NOADMIN_GROUP 100

// !help defines.
#ifdef ITA
    #define COMMANDLIST "^1Comandi che puoi utilizzare:\n"
    #define H_LEVEL "^1Il tuo livello e' ^2"
    #define H_BAN "^2!ban <numero/nick> [<motivo>] ^1: banna un player, con o senza motivo\n"
    #define H_UNBAN "^2!unban <id/nick> ^1: sbanna un player (id dato da !find)\n"
    #define H_KICK "^2!kick <numero/nick> ^1: butta fuori un player\n"
    #define H_MUTE "^2!mute <numero/nick/all> ^1: muta o smuta un singolo player, o tutti\n"
    #define H_FIND "^2!find <nick> ^1: cerca un player tra i bannati\n"
    #define H_FINDOP "^2!findop <nick> ^1: cerca un player tra gli admin\n"
    #define H_OP "^2!op <numero/nick> [level] ^1: da' lo stato di admin ad un player\n"
    #define H_DEOP "^2!deop <id> ^1: toglie lo stato di admin ad un player (id da !findop)\n"
    #define H_STRICT "^2!strict <off/0/1/2/3/4> ^1: cambia il livello di sicurezza del server\n"
    #define H_ADVANCEDCHECKS "^2!advanced <off/0/1/2> ^1: cambia i controlli avanzati del server\n"
    #define H_NUKE "^2!nuke <numero/nick> ^1: lancia un nuke ad un player\n"
    #define H_SLAP "^2!slap <numero/nick> [2-9] ^1: slappa una (o piu' volte) un player\n"
    #define H_VETO "^2!veto ^1: annulla la votazione in corso\n"
    #define H_STATUS "^2!status ^1: informazioni sullo stato del bot\n"
    #define H_FORCE "^2!<red/blue/spect> <numero/nick> ^1: cambia team ad un player\n"
    #define H_MAP "^2!map <nome> [nome2] ^1: cambia la mappa (per esempio, ut4_casa)\n"
    #define H_NEXTMAP "^2!nextmap <nome> [nome2] ^1: imposta la mappa successiva (vedi !map)\n"
    #define H_ADMINS "^2!admins ^1: elenca gli admin attualmente in gioco\n"
    #define H_PASS "^2!pass <pass> ^1: cambia la password al server\n"
    #define H_CONFIG "^2!config <nome> [nome2]  ^1: carica un file di configurazione\n"
    #define H_WARNINGS "^2!warnings <off/public/private> ^1: imposta come inviare i warnings\n"
    #define H_BANTIMEWARN "^2!bantimewarn <on/off> ^1: avvisi per vecchi ban su nick/ip\n"
    #define H_RESTART "^2!restart ^1: ricomincia la partita corrente.\n"
    #define H_RELOAD "^2!reload ^1: ricarica la mappa corrente.\n"
    #define H_BALANCE "^2!balance ^1: riequilibra le squadre.\n"
    #define H_GRAVITY "^2!grav <livello/off> ^1: cambia la gravita'.\n"
    #define H_CHANGELEVEL "^2!level <nome/id> <livello> ^1: cambia il livello di un admin.\n"
    #define H_BIGTEXT "^2!bigtext <message> ^1: stampa un testo in grande.\n"
    #define H_PLAYERSINFO "^2!pinfo ^1: mostra alcune informazioni dei giocatori online (rcon status).\n"
    #define H_TEAMS "^2!teams ^1: sistema il numero di giocatori per squadra.\n"
    #define H_CYCLE "^2!cycle ^1: carica la mappa successiva.\n"
    #define H_RULES "^2!rules ^1: mostra le regole del server.\n"
#else
    #define COMMANDLIST "^1You can use these commands:\n"
    #define H_LEVEL "^1Your level is ^2"
    #define H_BAN "^2!ban <number/nick> [<reason>] ^1: ban a player, with or without a reason\n"
    #define H_UNBAN "^2!unban <id/nick> ^1: unban a player (id from !find)\n"
    #define H_KICK "^2!kick <number/nick> ^1: kick a player\n"
    #define H_MUTE "^2!mute <number/nick/all> ^1: mute or unmute a player, or all\n"
    #define H_FIND "^2!find <nick> ^1: search a player from banned list\n"
    #define H_FINDOP "^2!findop <nick> ^1: search a player from admin list\n"
    #define H_OP "^2!op <number/nick> [level] ^1: register the player as admin\n"
    #define H_DEOP "^2!deop <id> ^1: unregister the player as admin (id from !findop)\n"
    #define H_STRICT "^2!strict <off/0/1/2/3/4> ^1: change the security level of the server\n"
    #define H_ADVANCEDCHECKS "^2!advanced <off/0/1/2> ^1: change the advanced checks of the server\n"
    #define H_NUKE "^2!nuke <number/nick> ^1: nuke a player\n"
    #define H_SLAP "^2!slap <number/nick> [2-9] ^1: slaps once (or more times) a player\n"
    #define H_VETO "^2!veto ^1: cancel the vote in progress\n"
    #define H_STATUS "^2!status ^1: information about the bot\n"
    #define H_FORCE "^2!<red/blue/spect> <number/nick> ^1: change team team of a player\n"
    #define H_MAP "^2!map <name> [name2] ^1: change map (for example, ut4_casa)\n"
    #define H_NEXTMAP "^2!nextmap <name> [name2] ^1: set the next map (see !map)\n"
    #define H_ADMINS "^2!admins ^1: show admins actually in game\n"
    #define H_PASS "^2!pass <pass> ^1: change server's password\n"
    #define H_CONFIG "^2!config <name> [name2] ^1: load a configuration file\n"
    #define H_WARNINGS "^2!warnings <off/public/private> ^1: sets how to send warnings\n"
    #define H_BANTIMEWARN "^2!bantimewarn <on/off> ^1: warnings for old nick/ip bans\n"
    #define H_RESTART "^2!restart ^1: restart the current game.\n"
    #define H_RELOAD "^2!reload ^1: reload the current map.\n"
    #define H_BALANCE "^2!balance ^1: balance teams.\n"
    #define H_GRAVITY "^2!grav <level/off> ^1: change gravity.\n"
    #define H_CHANGELEVEL "^2!level <nick/id> <level> ^1: change the admin's level.\n"
    #define H_BIGTEXT "^2!bigtext <message> ^1: write the message using a big font.\n"
    #define H_PLAYERSINFO "^2!pinfo ^1: shows infos of ingame players (rcon status).\n"
    #define H_TEAMS "^2!teams ^1: fixes the number of players per team.\n"
    #define H_CYCLE "^2!cycle ^1: load the next map.\n"
    #define H_RULES "^2!rules ^1: shows server's rules.\n"
#endif

class Analyzer
{
private:
    enum CheckTimingEnum{
        BANNED,
        WARNING,
        NOPE
    };
    
    Backup* backup;
    ConfigLoader * m_configLoader;
    std::ifstream *log;             //inputstream of servers's log.
    int contatore;                  //counts "empty" loop, without commands executed, used to decide if we have to enter the "relax" or "sleeping" mode
    int fascia;                     //actual operating mode (active/relax/sleeping)
    bool commandexecuted;           //used to see if i have executed a command or not.
    int giri;                       //it counts loops done: used to maintaining procedures, like to check if configuration files has changed.

    void loadOptions();             //loads new options.

    // funzioni associate ai regex (comandi dal server/eventi)
    void clientUserInfoChanged(char* line);
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
    void nuke(char* line);
    void status(char* line);
    void force(char* line);
    void iamgod(char* line);
    void map(char* line);
    void nextmap(char* line);
    void admins(char* line);
    void pass(char* line);
    void config(char* line);
    void warnings(char* line);
    void bantimewarn(char* line);
    void restart(char* line);
    void reload(char* line);
    void balance(char* line);
    void gravity(char* line);
    void changeLevel(char* line);
    void bigtext(char* line);
    void teams(char* line);
    void playersInfos(char* line);
    void cyclemap(char* line);
    void advancedChecks(char* line);
    void rules(char* line);

protected:
    bool isA(const char* line,const std::string &regex);          //testa se la riga soddisfa il regex.
    int isAdminSay(char *line, std::string &numero);		//returns the player's op level, and save his number in @numero.
    void getDateAndTime(std::string &date,std::string &time); //salva nelle stringhe data e ora.
    bool guidIsBanned(const std::string& guid, const std::string& nick, const std::string& numero, const std::string& ip);               //controlla se la guid è stata bannata.
    bool nickIsBanned(const std::string& nick, const std::string& numero, const std::string& ip, const std::string& guid);               //controlla se il nick è stato bannato.
    bool ipIsBanned(const std::string& ip, const std::string& numero, const std::string& nick, const std::string& guid);               //controlla se l'ip è stato bannato.
    void buttaFuori(const std::string& reason, const std::string numero, const std::string nick);
    std::vector<unsigned int> admins();                  //returns array's indexes of in-game players that are admin. *WARNING* do not use in threads operations.
    void tellToAdmins(std::string phrase);               //invia un messaggio a tutti gli admin attualmente in game (messaggio privato).
    int translatePlayer(std::string player);            //@player rappresenta un nick o una parte del nick di un player in game, la funzione restituisce l'indice nell'array dei giocatori indicato da @player, se non trovato o non univoco restituisce -1.
    int findPlayer(std::string number);                 //return the index of player with number @number if exists, or -1.
    int translateMap(std::string map, std::string map2); //returns the index of the server::serverMaps that contains the complete map name, or -1 if not found.
    int translateConfig(std::string conf, std::string conf2); //returns the index of the server::serverConfigs that contain the complete config name or -1 if not found.
    CheckTimingEnum checkTiming (const std::vector<Db::idMotiveStruct> &records, const Server::Timing &option);  //it decides if nick or ip is to considered banned.
    Connection * server;
    Db * database;
    ConfigLoader::Options* m_dati;                      //it contains all options of each server (players included).
    Scheduler * m_scheduler;                            //instance of scheduler class, it decides what command have to be executed.
    FileLister * m_fileLister;                          //generate list of maps and configs.


public:
    Analyzer( Connection *conn, Db *db, ConfigLoader *configLoader );
    ~Analyzer();
    void main_loop();
};

#endif
