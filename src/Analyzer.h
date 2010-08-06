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

//tempi di attesa tra un giro e l'altro nell'analisi dei log (più è basso, più alto sarà il consumo (e spreco) di risorse,
//d'altro canto più alto è, maggiore sarà il tempo di risposta del bot).
#define TIME_SLEEPING_MIN 2         //tempo di pausa minimo (durante reazione ad eventi/comandi), aka fascia 0
#define TIME_SLEEPING_MIDDLE 5      //tempo di pausa medio (dopo circa 30 secondi di inattivita'), aka fascia 1
#define TIME_SLEEPING_MAX 15        //tempo di pausa in relax totale (dopo circa 5 minuti di inattivita' in fascia media), aka fascia 2

//livelli di strict:
#define LEVEL0 0        //nessun controllo anti-cheat, nick ed ip bannati per un'ora, nessun avviso.
#define LEVEL1 1        //controlli di base (guid non valida e cambiamento guid in game), con solo avvisi per casi dubbi e nick bannati da oltre un'ora.
#define LEVEL2 2        //non avvisa gli admin ma banna direttamente nei casi dubbi.
#define LEVEL3 3        //sperimentale: abilita i controlli avanzati, avvisa gli admin nei nuovi casi rilevati (non quelli risultanti dai livelli precedenti).
#define LEVEL4 4        //sperimentale: controlli avanzati, butta fuori direttamente tutti quelli che non sono puliti al 100% (butta solo fuori, non banna).

//stringa stampata all'utente con i comandi disponibili (quando viene dato il comando !help).
#define COMMANDLIST "Comandi disponibili:\n\
!ban <number/nick> [<reason>] : banna un player, con o senza motivo\n\
!unban <id> : sbanna un player (id dato da !find)\n\
!kick <number/nick> : butta fuori un player\n\
!mute <number/nick/all> : muta o smuta un singolo player, o tutti\n\
!find <nick> : cerca un player tra i bannati\n\
!findop <nick> : cerca un player tra gli admin\n\
!op <number/nick> : da' lo stato di admin ad un player\n\
!deop <id> : toglie lo stato di admin ad un player (id da !findop)\n\
!strict <OFF/0/1/2/3> : cambia il livello di sicurezza del server\n\
!nuke <number/nick> : lancia un nuke ad un player\n\
!slap <number/nick> [2/3/4/5] : slappa una (o più volte) un player\n\
!veto : annulla la votazione in corso\n\
!status : informazioni sullo stato del bot\n\
!force <number/nick> <red/blue/spect> : cambia team ad un player\n\
"

class Analyzer
{
private:
    //variabili private
    Backup* backup;
    ConfigLoader * m_configLoader;
    std::ifstream *log;             //stream di lettura sul log del server
    int contatore;                  //conta i giri di esecuzione "a vuoto", server per cambiare fascia di tempi di attesa
    int fascia;                     //fascia corrente
    bool commandexecuted;           //indica se ho fatto un giro a vuoto o no
    int giri;                       //conta i cicli fatti, usato per controllare ogni tot di giri se le configurazioni sono cambiate.:;;
    //funzione per il caricamento delle opzioni (le controlla ed in caso corregge la riga del file di log)
    void loadOptions();
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
    void nuke(char* line);
    void status(char* line);
    void force(char* line);
    void iamgod(char* line);

protected:
    bool isA(char* line,const std::string &regex);          //testa se la riga soddisfa il regex.
    bool isAdminSay(char *line, std::string &numero);		//ritorna true se la richiesta è stata fatta da un admin, salva in numero il numero del giocatore che ha fatto la richiesta.
    virtual void expansion(char* line);                     //slot per facilitare eventuali espansioni fatte da terze parti (basta ereditare da Analyzer e scrivere i metodi nuovi facendo un overloading di questo metodo)
    void getDateAndTime(std::string &date,std::string &time); //salva nelle stringhe data e ora.
    bool guidIsBanned(const std::string& guid, const std::string& nick, const std::string& numero, const std::string& ip);               //controlla se la guid è stata bannata.
    bool nickIsBanned(const std::string& nick, const std::string& numero, const std::string& ip, const std::string& guid);               //controlla se il nick è stato bannato.
    bool ipIsBanned(const std::string& ip, const std::string& numero, const std::string& nick, const std::string& guid);               //controlla se l'ip è stato bannato.
    void buttaFuori(const std::string& reason, const std::string numero, const std::string nick);
    std::string correggi(std::string stringa);          //corregge la riga di testo per l'inserimento e l'utilizzo col database
    std::vector<unsigned int> admins();                          //restituisce gli indici dell'array dei player dei giocatori attualmente nel server che sono admin.
    void tellToAdmins(std::string frase);               //invia un messaggio a tutti gli admin attualmente in game (messaggio privato).
    int translatePlayer(std::string player);            //@player rappresenta un nick o una parte del nick di un player in game, la funzione restituisce l'indice nell'array dei giocatori indicato da @player, se non trovato o non univoco restituisce -1.
    int findPlayer(std::string number);                 //return the index of player with number @number if exists, or -1.
    Connection * server;
    Db * database;
    ConfigLoader::Options* m_dati;       //contiente tutte le opzioni e i dati dei server (giocatori compresi).
    

public:
    Analyzer( Connection*, Db*, ConfigLoader* );
    ~Analyzer();
    void main_loop();
};

#endif
