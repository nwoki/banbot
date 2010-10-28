/*
    Analyzer.cpp is part of BanBot.

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


// put in h files
#ifndef _Analyzer_cpp_
#define _Analyzer_cpp_

#include "Analyzer.h"
#include "InstructionsBlock.h"

#include <unistd.h>
#include <regex.h>

//#include <time.h>
//#include <stdlib.h>
//#include <sys/stat.h>

//defines per i regex:
#define _R_CLIENT_CONNECT " *[0-9]+:[0-9]{2} +ClientConnect:"
#define _R_CLIENT_USER_INFO " *[0-9]+:[0-9]{2} +ClientUserinfo:"
#define _R_COMPLETE_CLIENT_USER_INFO " *[0-9]+:[0-9]{2} +ClientUserinfo: +[0-9]+ +\\ip\\[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}:[0-9]{1,6}\\name\\[^ \t\n\r\f\v]+\\racered\
\\[0-9]{1}\\raceblue\\[0-9]{1}\\rate\\[0-9]+\\ut_timenudge\\[0-9]+\\cg_rgb\\[0-9]{1,3} [0-9]{1,3} [0-9]{1,3}\\cg_predictitems\\[01]{1}\\cg_physics\\[01]{1}\\snaps\\[0-9]{1,2}\\model\\[^ \t\n\r\f\v]+\
\\headmodel\\[^ \t\n\r\f\v]+\\team_model\\[^ \t\n\r\f\v]+\\team_headmodel\\[^ \t\n\r\f\v]+\\color1\\[0-9]{1,2}\\color2\\[0-9]{1,2}\\handicap\\100\\sex\\[^ \t\n\r\f\v]+\\cl_anonymous\\[01]{1}\
\\gear\\[^ \t\n\r\f\v]+\\teamtask\\[0-9]+\\cl_guid\\[A-F0-9]{32}\\weapmodes\\[0-2]{20}"

#define _R_CLIENT_DISCONNECT "^ *[0-9]+:[0-9]{2} +ClientDisconnect:"
#define _R_BAN "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!ban [^\t\n\r\f\v]+"
#define _R_BAN_NUMBER "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!ban [0-9]{1,2}"
#define _R_FIND "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!find [^ \t\n\r\f\v]+$"
#define _R_FINDOP "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!findop [^ \t\n\r\f\v]+$"
#define _R_UNBAN "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!unban [0-9]+$"
#define _R_OP "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!op [^ \t\n\r\f\v]+$"
#define _R_OP_NUMBER "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!op [0-9]{1,2}$"
#define _R_DEOP "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!deop [0-9]+$"
#define _R_GUID "[A-F0-9]{32}"
#define _R_INITGAME "^ *[0-9]+:[0-9]{2} +InitGame:"
#define _R_HELP "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!help$"
#define _R_KICK "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!kick [^ \t\n\r\f\v]+$"
#define _R_KICK_NUMBER "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!kick [0-9]{1,2}$"
#define _R_MUTE " *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!mute [^ \t\n\r\f\v]+$"
#define _R_MUTE_ALL "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!mute (all)|(ALL)$"
#define _R_MUTE_NUMBER "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!mute [0-9]{1,2}$"
#define _R_STRICT "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!strict (OFF|off|[0-2]{1})$"
#define _R_VETO "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!veto$"
#define _R_SLAP "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!slap [^\t\n\r\f\v]+$"
#define _R_SLAP_NUMBER "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!slap [0-9]{1,2}"
#define _R_SLAP_MORE "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!slap [^ \t\n\r\f\v]+ [2-5]{1}$"
#define _R_NUKE "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!nuke [^ \t\n\r\f\v]+$"
#define _R_NUKE_NUMBER "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!nuke [0-9]{1,2}$"
#define _R_COMMAND "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +![^\t\n\r\f\v]+$"
#define _R_STATUS "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!status$"
#define _R_FORCE "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!force (red|blue|spect) [^ \t\n\r\f\v]+$"
#define _R_FORCE_NUMBER "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!force (red|blue|spect) [0-9]{1,2}$"
#define _R_IAMGOD "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!iamgod$"
#define _R_MAP "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!map [^ \t\n\r\f\v]+$"
#define _R_NEXTMAP "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!nextmap [^ \t\n\r\f\v]+$"
#define _R_ADMINS "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!admins$"
#define _R_PASS "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!pass [^ \t\n\r\f\v]+$"
#define _R_CONFIG "^ *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!config [^ \t\n\r\f\v]+$"

//costruttore
Analyzer::Analyzer(Connection* conn, Db* db, ConfigLoader* configLoader )
    : backup( new Backup( configLoader->getOptions(), db ) )
    , m_configLoader( configLoader )
    , log(new ifstream())
    , contatore(0)
    , fascia(0)
    , giri(0)
    , server( conn )
    , database( db )
    , m_dati( configLoader->getOptions() )
    , m_scheduler( new Scheduler( m_dati , server ) )
{
    loadOptions();

    #ifdef ITA
        std::cout<<"[OK] Analyzer inizializzato.\n";
        *(m_dati->errors)<<"[OK] Analyzer inizializzato.\n\n";
    #else
        std::cout<<"[OK] Analyzer initialized.\n";
        *(m_dati->errors)<<"[OK] Analyzer initialized.\n\n";
    #endif
    sleep(1);
    m_dati->errors->close();
}

//distruttore
Analyzer::~Analyzer()
{
    if( log->is_open() ) log->close();
    delete log;
    m_dati->errors->close();
    m_dati->log->close();
    delete m_configLoader;
    delete m_scheduler;
}

//caricatore delle opzioni
void Analyzer::loadOptions()
{
    //se i dati sono cambiati, vado alla fine del file di log
    for (unsigned int i = 0; i < m_dati->size(); i++ )
    {
        if( (*m_dati)[i].isChanged() )
        {
            ifstream * temp=new ifstream();
            temp->open((*m_dati)[i].serverLog().c_str());
            if ( temp->is_open() )
            {
                temp->seekg ( 0, ios:: end );
                (*m_dati)[i].setRow( temp->tellg() );
                #ifdef DEBUG_MODE
                    #ifdef ITA
                        std::cout << "Valore di partenza del file "<<i<<": "<<(*m_dati)[i].row()<<" : "<<temp->tellg()<<" \n";
                    #else
                        std::cout << "Starting value for file "<<i<<": "<<(*m_dati)[i].row()<<" : "<<temp->tellg()<<" \n";
                    #endif
                #endif
            }
            else (*m_dati)[i].setRow( 0 );
            delete temp;
            (*m_dati)[i].setChanged(false);
        }
  }
  database->checkDatabases();

  #ifdef ITA
        std::cout<<"Nuove opzioni caricate.\n";
        m_dati->errors->timestamp();
        *(m_dati->errors)<<"\nNuove opzioni caricate.\n";
  #else
        std::cout<<"New options loaded.\n";
        m_dati->errors->timestamp();
        *(m_dati->errors)<<"\nNew options loaded.\n";
  #endif
}

//testa l'array di caratteri passato col regex, torna true se la condizione imposta dal regex è soddisfatta.
bool Analyzer::isA( char* line, const std::string& regex )
{
    regex_t r;

    if ( regcomp( &r, regex.c_str(), REG_EXTENDED|REG_NOSUB ) == 0){
        int status = regexec( &r, line, ( size_t )0, NULL, 0 );
        regfree( &r );
        if( status == 0 )
            return true;
    }
    return false;
}

bool Analyzer::isAdminSay( char* line, std::string &numero )
{
    //controllo se la persona che l'ha richiesto ha i permessi per farlo
    std::string temp = line;
    int pos = temp.find( "say:" );
    pos = temp.find_first_not_of( " ", pos+4 );
    int end = temp.find_first_of( " ", pos );
    numero = temp.substr( pos, end-pos );

    //dal numero del richiedente, mi prendo il guid

    int i = findPlayer( numero );
    if ( i >= 0 )
    {
        #ifdef ITA
            std::cout<<" richiesto da "<<(*m_dati)[m_dati->serverNumber][i]->nick<<", "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
            *(m_dati->log)<<" richiesto da "<<(*m_dati)[m_dati->serverNumber][i]->nick<<", "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
        #else
            std::cout<<" requested by "<<(*m_dati)[m_dati->serverNumber][i]->nick<<", "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
            *(m_dati->log)<<" requested by "<<(*m_dati)[m_dati->serverNumber][i]->nick<<", "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
        #endif
        if( database->checkAuthGuid( correggi((*m_dati)[m_dati->serverNumber][i]->GUID)) )
            return true;
    }
    else
    {
        #ifdef ITA
            std::cout<<" richiesto da uno sconosciuto. Lo ignoro.\n";
            *(m_dati->log)<<" richiesto da uno sconosciuto. Lo ignoro.\n";
        #else
            std::cout<<" requested by unknown. I'll ignore it.\n";
            *(m_dati->log)<<" requested by unknown. I'll ignore it.\n";
        #endif
    }
    return false;
}

//funzione da sovrascrivere nelle eventuali espansioni
void Analyzer::expansion(char* line)
{
}

void Analyzer::clientUserInfo(char* line)
{
    InstructionsBlock * block = new InstructionsBlock();    // create block for instructions
    // prendo il numero giocatore e la guid, utilizzando le funzioni delle stringhe
    std::string temp=line;
    int pos=temp.find("ClientUserinfo:");
    pos=temp.find_first_not_of(' ',pos+15);
    int end=temp.find_first_of(' ',pos);
    std::string numero=temp.substr(pos,end-pos);
    pos=temp.find("ip");
    pos=temp.find_first_not_of("\\",pos+3);
    end=temp.find_first_of("\\ ",pos);
    std::string ip=temp.substr(pos,end-pos);
    pos=temp.find("name");
    pos=temp.find_first_not_of("\\",pos+4);
    end=temp.find_first_of("\\",pos);        //permetto anche spazi all'interno del nome, tutti i caratteri permessi tranne lo slash
    std::string nick=temp.substr(pos,end-pos);
    pos=temp.find("cl_guid");
    pos=temp.find_first_not_of("\\",pos+7);
    end=temp.find_first_of("\\ ",pos);
    std::string guid;
    if (pos!=end) guid=temp.substr(pos,end-pos);

    #ifdef ITA
        std::cout<<"[-]Estrapolati i dati: numero="<<numero<<" guid="<<guid<<" nick="<<nick<<" ip="<<ip<<"\n";
        (m_dati->log)->timestamp();
        *(m_dati->log)<<"\n[-]Estrapolati i dati: numero="<<numero<<" guid="<<guid<<" nick="<<nick<<" ip="<<ip<<"\n";
    #else
        std::cout<<"[-]Extracted data: number="<<numero<<" guid="<<guid<<" nick="<<nick<<" ip="<<ip<<"\n";
        (m_dati->log)->timestamp();
        *(m_dati->log)<<"\n[-]Extracted data: number="<<numero<<" guid="<<guid<<" nick="<<nick<<" ip="<<ip<<"\n";
    #endif

    //cerco il giocatore giusto all'interno della mia lista, e salvo il guid nelle info
    bool kicked=false;

    int i = findPlayer( numero );
    if ( i >= 0 )
    {
        if ((!(*m_dati)[m_dati->serverNumber][i]->GUID.empty() && (*m_dati)[m_dati->serverNumber][i]->GUID.compare(guid)!=0) || guid.empty())
        {
            if (!guid.empty() && (*m_dati)[m_dati->serverNumber].strict() >= LEVEL1)
            {
                // cambio illegale del GUID => cheats
                kicked=true;

                #ifdef ITA
                    std::cout<<"  [!] ban automatico per cheats (GUID cambiata durante il gioco).\n";
                    *(m_dati->log)<<"  [!] ban automatico per cheats (GUID cambiata durante il gioco).\n";
                #else
                    std::cout<<"  [!] automated ban for cheats (GUID changed during the game).\n";
                    *(m_dati->log)<<"  [!] automated ban for cheats (GUID changed during the game).\n";
                #endif
                std::string frase("^0BanBot: ^1auto-banning player number ");
                frase.append(numero);
                frase.append(", ");
                frase.append(nick);
                frase.append(" for cheats.");
                block->say(frase);
                std::string ora;
                std::string data;
                std::string motivo("auto-ban 4 cheats.");
                getDateAndTime(data,ora);
                //non prendo la guid attuale, ma quella precedente (che spesso è quella vera e propria dell'utente prima che attivi i cheat)
                database->ban(correggi(nick),ip,data,ora,correggi((*m_dati)[m_dati->serverNumber][i]->GUID),correggi(motivo),std::string());
                block->kick(numero);
            }
        }
        else
        {
            //tutto a posto =)
            //salvo guid e nick, inserisco il nick e l'ip nel db
            (*m_dati)[m_dati->serverNumber][i]->nick=nick;
            (*m_dati)[m_dati->serverNumber][i]->GUID=guid;
            (*m_dati)[m_dati->serverNumber][i]->ip=ip;
        }
    }
    else
    {
        //se non era presente tra i giocatori in memoria (avvio del bot a metà partita, linea mancante nel log ecc.) lo aggiungo adesso.
        Server::Player * gioc=new Server::Player();
        gioc->number=numero;
        gioc->GUID=guid;
        gioc->ip=ip;
        gioc->nick=nick;
        (*m_dati)[m_dati->serverNumber].push_back(gioc);
        i = (*m_dati)[m_dati->serverNumber].size() - 1;
    }

    //se la guid è vuota, sono cazzi amari...
    if (guid.empty() && (*m_dati)[m_dati->serverNumber].strict() >= LEVEL1)
    {
        if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL2 )
        {
            //guid vuota, probabili cheats, sono in modalità strict, butto fuori (provo a bannare un'eventuale guid precedente).
            kicked=true;
            #ifdef ITA
                std::cout<<"  [!] ban automatico per GUID non valida (vuota).\n";
                *(m_dati->log)<<"  [!] ban automatico per GUID non valida (vuota).\n";
            #else
                std::cout<<"  [!] automated ban for non-valid GUID (empty).\n";
                *(m_dati->log)<<"  [!] automated ban for non-valid GUID (empty).\n";
            #endif

            #ifdef ITA
                std::string frase("^0BanBot: ^1auto-ban al player numero ");
            #else
                std::string frase("^0BanBot: ^1auto-ban player number ");
            #endif
            frase.append(numero);
            frase.append(", ");
            frase.append(nick);
            #ifdef ITA
                frase.append(" per QKey corrotta.");
            #else
                frase.append(" for corrupted QKey.");
            #endif

            block->say(frase);
            std::string ora;
            std::string data;
            std::string motivo("auto-ban 4 corrupted QKey.");
            getDateAndTime(data,ora);
            //guardo se aveva una guid impostata precedentemente, in caso la banno.
            if (!(*m_dati)[m_dati->serverNumber][i]->GUID.empty())
                database->ban(correggi(nick),ip,data,ora,correggi((*m_dati)[m_dati->serverNumber][i]->GUID),correggi(motivo),std::string());
            //altrimenti banno solo in nickIsBanned
            else database->insertNewBanned(correggi(nick),ip,data,ora,correggi(motivo),std::string());
            block->kick(numero);
        }
        else
        {
            //guid vuota, ma non sono in strict mode. Semplicemente avviso gli amministratori.
            #ifdef ITA
                std::string frase("^0BanBot ^1attenzione: il player numero ");
            #else
                std::string frase("^0BanBot ^1warning: player number ");
            #endif
            frase.append(numero);
            frase.append(", ");
            frase.append(nick);
            #ifdef ITA
                frase.append(" ha la QKey corrotta:\n^1probabilmente sta usando dei cheats.");
            #else
                frase.append(" has a corrupted QKey:\n^1probably he's using cheats.");
            #endif
            tellToAdmins(frase);
        }
    }

    //se è tutto a posto procedo coi controlli
    if (!kicked)
    {
        //faccio un pò di controlli:
        //controllo che non sia stato già bannato
        //controllo se la guid, il nick e l'ip sono bannati
        if( !( guidIsBanned( guid, nick, numero, ip) || nickIsBanned( nick, numero, ip, guid ) || ipIsBanned( ip, numero, nick, guid )) )
        {
            //ok, non è stato bannato (per il momento). Controllo se ha un GUID valido.
            if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL1 && !guid.empty() && !isA(line, _R_GUID) )
            {
                //il guid è illegale, ban diretto
                #ifdef ITA
                    std::cout<<"  [!] ban automatico per GUID illegale\n";
                    *(m_dati->log)<<"  [!] ban automatico per GUID illegale\n";
                    std::string frase("^0BanBot: ^1kick automatico al giocatore ");
                #else
                    std::cout<<"  [!] automated ban for illegal GUID.\n";
                    *(m_dati->log)<<"  [!] automated ban for illegal GUID.\n";
                    std::string frase("^0BanBot: ^1kicking player number ");
                #endif

                frase.append(numero);
                frase.append(", ");
                frase.append(nick);
                #ifdef ITA
                    frase.append(" per cheats.");
                #else
                    frase.append(" for cheats.");
                #endif
                block->say(frase);
                std::string ora;
                std::string data;
                std::string motivo("auto-ban 4 cheats.");
                getDateAndTime(data,ora);
                database->ban(correggi(nick),ip,data,ora,correggi(guid),correggi(motivo),std::string());
                block->kick(numero);
            }
            else
            {
                //fin qua, tutto a posto. Controlli avanzati:
                if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL3 && !isA( line,_R_COMPLETE_CLIENT_USER_INFO ) )
                {
                    if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL4 )
                    {
                        //butto fuori
                        #ifdef ITA
                            std::cout<<"  [!] kick automatico per client non pulito (controlli avanzati non superati).\n";
                            *(m_dati->log)<<"  [!] kick automatico per client non pulito (controlli avanzati non superati).\n";
                            std::string frase("^0BanBot: ^1auto-kick al giocatore ");
                        #else
                            std::cout<<"  [!] automated kick for illegal client ( advanced checks not passed ).\n";
                            *(m_dati->log)<<"  [!] automated kick for illegal client ( advanced checks not passed ).\n";
                            std::string frase("^0BanBot: ^1auto-kicking player number ");
                        #endif
                        frase.append(numero);
                        frase.append(", ");
                        frase.append(nick);
                        #ifdef ITA
                            frase.append(" per client non pulito.");
                        #else
                            frase.append(" for illegal client.");
                        #endif
                        block->say(frase);
                        block->kick(numero);
                    }
                    else
                    {
                        //avviso solo gli admin
                        #ifdef ITA
                        std::string frase("^0BanBot ^1attenzione: il giocatore numero ");
                        #else
                        std::string frase("^0BanBot ^1warning: player number ");
                        #endif
                        frase.append(numero);
                        frase.append(", ");
                        frase.append(nick);
                        #ifdef ITA
                            frase.append(" ha un client non pulito.");
                        #else
                            frase.append(" has an illegal client.");
                        #endif
                        tellToAdmins(frase);
                    }
                }
                else
                {
                    #ifdef ITA
                        std::cout<<"  [OK] è a posto.\n";
                        *(m_dati->log)<<"  [OK] è a posto.\n";
                    #else
                        std::cout<<"  [OK] (s)he's ok.\n";
                        *(m_dati->log)<<"  [OK] (s)he's ok.\n";
                    #endif
                }
            }
        }
    }
    if( !block->isEmpty() )
        m_scheduler->addInstructionBlock( block, Server::HIGH );  // add to scheduler
    //ho finito le azioni in caso di clientUserinfo
}

void Analyzer::clientConnect(char* line)
{
    //prendo il numero del giocatore e mi creo il nuovo player corrispondente in memoria
    std::string temp=line;
    int pos=temp.find("ClientConnect:");
    pos=temp.find_first_not_of(" ",pos+14);
    int end=temp.find_first_of(" \n\0",pos);
    std::string numero=temp.substr(pos,end-pos);

    #ifdef ITA
        std::cout<<"[-] Nuovo client connesso: "<<numero<<"\n";
        (m_dati->log)->timestamp();
        *(m_dati->log)<<"\n[-] Nuovo client connesso: "<<numero<<"\n";
    #else
        std::cout<<"[-] New client connected: "<<numero<<"\n";
        (m_dati->log)->timestamp();
        *(m_dati->log)<<"\n[-] New client connected: "<<numero<<"\n";
    #endif
    //per pignoleria controllo che non sia già presente

    int i = findPlayer( numero );
    if ( i >= 0 )
    {
        //se l'ho trovato, resetto i dati in memoria
        (*m_dati)[m_dati->serverNumber][i]->GUID.clear();
        (*m_dati)[m_dati->serverNumber][i]->ip.clear();
        (*m_dati)[m_dati->serverNumber][i]->nick.clear();
    }
    else
    {
        //se non è presente, lo inserisco
        Server::Player * gioc=new Server::Player();
        gioc->number=numero;
        (*m_dati)[m_dati->serverNumber].push_back(gioc);
    }
    //ho finito le azioni in caso di clientConnect
}

void Analyzer::clientDisconnect(char* line)
{
    //prendo il numero del giocatore e lo elimino
    std::string temp=line;
    int pos=temp.find("ClientDisconnect:");
    pos=temp.find_first_not_of(" ",pos+17);
    int end=temp.find_first_of(" \n\0",pos);
    std::string numero=temp.substr(pos,end-pos);

    #ifdef ITA
        std::cout<<"[-] Client disconnesso: "<<numero<<"\n";
        (m_dati->log)->timestamp();
        *(m_dati->log)<<"\n[-] Client disconnesso: "<<numero<<"\n";
    #else
        std::cout<<"[-] Client disconnected: "<<numero<<"\n";
        (m_dati->log)->timestamp();
        *(m_dati->log)<<"\n[-] Client disconnected: "<<numero<<"\n";
    #endif
    //cerco il player e lo elimino
    int i = findPlayer( numero );
    if ( i >= 0 )
    {
        //trovato. elimino prima l'oggetto puntato
        delete (*m_dati)[m_dati->serverNumber][i];
        //elimino l'elemento in vector: prendo l'iteratore
        std::vector<Server::Player*>::iterator iteratore=(*m_dati)[m_dati->serverNumber].begin();
        //scorro fino all'elemento corretto
        for (int j=0; j<i;j++) iteratore++;
        //elimino l'elemento
        (*m_dati)[m_dati->serverNumber].erase(iteratore);
    }
    //finite le azioni in caso di disconnect
}

void Analyzer::ban(char* line)
{
    std::cout<<"[!] Ban";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Ban";
    //controllo se ho trovato il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        #ifdef ITA
            std::cout<<"  [OK] è un admin. Applico il ban.\n";
            *(m_dati->log)<<"  [OK] è un admin. Applico il ban.\n";
        #else
            std::cout<<"  [OK] Is an admin. Applying ban.\n";
            *(m_dati->log)<<"  [OK] Is an admin. Applying ban.\n";
        #endif
        //ok ha i permessi, eseguo.
        int i=0;
        //prendo il numero del giocatore da bannare
        std::string temp(line);
        unsigned int pos=temp.find("!ban");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+4);
        int end=temp.find_first_of(" \n",pos);
        std::string player=temp.substr(pos,end-pos);
        pos=temp.find_first_not_of(" ",end);
        std::string motivo;
        if ( pos<temp.size() ) motivo=temp.substr(pos,temp.size()-pos);
        if ( isA(line,_R_BAN_NUMBER) )
            i = findPlayer( player );
        else
            i = translatePlayer( player );
        //se ho il guid, banno il player (le operazioni non sono eseguite "in diretta",
        //per pignoleria controllo anche se per una rarissima combinazione non è già bannato).
        if (i>=0 && !database->checkBanGuid((*m_dati)[m_dati->serverNumber][i]->GUID))
        {
            if (!database->checkAuthGuid((*m_dati)[m_dati->serverNumber][i]->GUID))
            {
                #ifdef ITA
                    std::cout<<"  [+]banno "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" con guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    *(m_dati->log)<<"  [+]banno "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" con guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    std::string frase("^0BanBot: ^1banno il giocatore ");
                #else
                    std::cout<<"  [+]banning "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" with guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    *(m_dati->log)<<"  [+]banning "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" with guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    std::string frase("^0BanBot: ^1banning player number ");
                #endif
                frase.append((*m_dati)[m_dati->serverNumber][i]->number);
                frase.append(", ");
                frase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                if (!motivo.empty())
                {
                    #ifdef ITA
                        frase.append(" per ");
                    #else
                        frase.append(" for ");
                    #endif
                    frase.append(motivo);
                }
                frase.append(".");
                block->say(frase);
                std::string ora;
                std::string data;
                getDateAndTime(data,ora);
                unsigned int j=0;
                while (j<(*m_dati)[m_dati->serverNumber].size() && (*m_dati)[m_dati->serverNumber][j]->number.compare(numeroAdmin)!=0) j++;
                if (database->ban(correggi((*m_dati)[m_dati->serverNumber][i]->nick),(*m_dati)[m_dati->serverNumber][i]->ip,data,ora,correggi((*m_dati)[m_dati->serverNumber][i]->GUID),correggi(motivo),correggi((*m_dati)[m_dati->serverNumber][j]->GUID)))
                {
                    #ifdef ITA
                        std::cout<<"  [OK] giocatore bannato\n";
                        *(m_dati->log)<<"  [OK] giocatore bannato\n";
                    #else
                        std::cout<<"  [OK] player banned\n";
                        *(m_dati->log)<<"  [OK] player banned\n";
                    #endif
                }
                else
                {
                    #ifdef ITA
                        std::cout<<"  [FAIL] errore nella chiamata al database\n";
                        *(m_dati->log)<<"  [FAIL] errore nella chiamata al database\n";
                        (m_dati->errors)->timestamp();
                        *(m_dati->errors)<<"\n  [FAIL] In " << (*m_dati)[m_dati->serverNumber].name()  << " : errore nella chiamata al database\n";
                    #else
                        std::cout<<"  [FAIL] error on db call\n";
                        *(m_dati->log)<<"  [FAIL] error on db call\n";
                        (m_dati->errors)->timestamp();
                        *(m_dati->errors)<<"\n  [FAIL] On " << (*m_dati)[m_dati->serverNumber].name()  << " : error on db call\n";
                    #endif
                }
                block->kick((*m_dati)[m_dati->serverNumber][i]->number);
            }
            else
            {
                #ifdef ITA
                    std::cout<<"  [!]fail: il giocatore è un admin\n";
                    *(m_dati->log)<<"  [!]fail: il giocatore è un admin\n";
                    block->tell("^1Banning error: è un admin.",numeroAdmin);
                #else
                    std::cout<<"  [!]fail: player is an admin\n";
                    *(m_dati->log)<<"  [!]fail: player is an admin\n";
                    block->tell("^1Banning error: (s)he's an admin.",numeroAdmin);
                #endif
            }
        }
        else
        {
            #ifdef ITA
                std::cout<<"  [!]fail: giocatore non in gioco (o nick sbagliato) o già bannato\n";
                *(m_dati->log)<<"  [!]fail: giocatore non in gioco (o nick sbagliato) o già bannato\n";
                block->tell("^1Banning error: numero del giocatore sbagliato o nick non univoco.",numeroAdmin);
            #else
                std::cout<<"  [!]fail: player not in-game (or wrong nick) or already banned\n";
                *(m_dati->log)<<"  [!]fail: player not in-game (or wrong nick) or already banned\n";
                block->tell("^1Banning error: numbero of the player wrong or nick non unique.",numeroAdmin);
            #endif
        }
        m_scheduler->addInstructionBlock( block, Server::HIGH );
    }
}

void Analyzer::unban(char* line)
{
    std::cout<<"[!] Unban";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Unban";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();    // prepare instructionBlock
        //prendo l'identificativo da sbannare
        std::string temp(line);
        int pos=temp.find("!unban");
        pos=temp.find_first_of("0123456789",pos+6);
        int end=temp.find_first_of(" ",pos);
        std::string numero=temp.substr(pos,end-pos);

        //ho il numero, elimino dal database tutti i record relativi.
        std::string frase;
        #ifdef ITA
            if (database->deleteBanned(numero))
                frase.append("^0BanBot: ^1utente sbannato con successo.");
            else frase.append("^0BanBot: ^1è stato riscontrato un errore, utente non sbannato.");
        #else
            if (database->deleteBanned(numero))
                frase.append("^0BanBot: ^1player successfully unbanned.");
            else frase.append("^0BanBot: ^1error: player not unbanned.");
        #endif
        block->tell(frase,numeroAdmin);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::find(char* line)
{
    std::cout<<"[!] Find";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Find";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numero;
    if (isAdminSay(line,numero))
    {
        InstructionsBlock * block = new InstructionsBlock();
        #ifdef ITA
            std::cout<<"  [OK] è un admin. Eseguo il find.\n";
            *(m_dati->log)<<"  [OK] è un admin. Eseguo il find.\n";
        #else
            std::cout<<"  [OK] Is an admin. Doing find.\n";
            *(m_dati->log)<<"  [OK] Is an admin. Doing find.\n";
        #endif

        //estraggo il nick da cercare.
        std::string temp(line);
        int pos=temp.find("!find");
        std::string nick=temp.substr(pos+6);

        //ho il nick da cercare
        #ifdef ITA
            std::cout<<"  [-]Ricerca per "<<nick<<".\n";
            *(m_dati->log)<<"  [-]Ricerca per "<<nick<<".\n";
        #else
            std::cout<<"  [-]Searching for "<<nick<<".\n";
            *(m_dati->log)<<"  [-]Searching for "<<nick<<".\n";
        #endif
        //eseguo la ricerca sul DB e invio i risultati al server di gioco.
        std::vector<Db::idNickMotiveAuthorStruct> risultato=database->findPreciseIdMotiveAuthorViaNickBanned(nick);
        std::vector<Db::idNickMotiveAuthorStruct> risultatoApprossimativo=database->findAproxIdMotiveAuthorViaNickBanned(nick);

        #ifdef ITA
            std::cout<<"ricerca: "<<risultato.size()<<" "<<risultatoApprossimativo.size()<<"\n";
            std::string frase("^2Risultati esatti: \n^1");
        #else
            std::cout<<"search: "<<risultato.size()<<" "<<risultatoApprossimativo.size()<<"\n";
            std::string frase("^2Exact results: \n^1");
        #endif
        if (risultato.size()>5)
        {
            #ifdef ITA
                frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                frase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            frase.append(risultato[0].id);
            frase.append("^2:^1 ");
            frase.append(risultato[0].nick);
            frase.append(" ");
            frase.append(risultato[0].motive);
            frase.append(" ^2by^1 ");
            frase.append(risultato[0].author);
            frase.append(" ^2?");
        }
        else
        {
            if (risultato.empty()) frase.append("none.");
            else for (unsigned int i=0; i<risultato.size();i++)
            {
                frase.append(risultato[i].id);
                frase.append("^2:^1 ");
                frase.append(risultato[i].nick);
                frase.append(" ");
                frase.append(risultato[i].motive);
                frase.append(" ^2by^1 ");
                frase.append(risultato[i].author);
                if(i<risultato.size()-1) frase.append(",\n^1");
                else frase.append(".");
            }
        }
        block->tell(frase,numero);

        frase.clear();
        #ifdef ITA
            frase.append("^2Risultati ricerca: \n^1");
        #else
            frase.append("^2Search results: \n^1");
        #endif
        if (risultatoApprossimativo.size()>15)
        {
            #ifdef ITA
                frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                frase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            frase.append(risultatoApprossimativo[0].id);
            frase.append("^2:^1 ");
            frase.append(risultatoApprossimativo[0].nick);
            frase.append(" ");
            frase.append(risultatoApprossimativo[0].motive);
            frase.append(" ^2by^1 ");
            frase.append(risultatoApprossimativo[0].author);
            #ifdef ITA
                frase.append("\n^2, o forse \n ^1");
            #else
                frase.append("\n^2, or \n ^1");
            #endif
            frase.append(risultatoApprossimativo[1].id);
            frase.append("^2:^1 ");
            frase.append(risultatoApprossimativo[1].nick);
            frase.append(" ");
            frase.append(risultatoApprossimativo[1].motive);
            frase.append(" ^2by^1 ");
            frase.append(risultatoApprossimativo[1].author);
            frase.append(" ^2?");
        }
        else
        {
            if (risultatoApprossimativo.empty()) frase.append("none.");
            else for (unsigned int i=0; i<risultatoApprossimativo.size();i++)
            {
                frase.append(risultatoApprossimativo[i].id);
                frase.append("^2:^1 ");
                frase.append(risultatoApprossimativo[i].nick);
                frase.append(" ");
                frase.append(risultatoApprossimativo[i].motive);
                frase.append(" ^2by^1 ");
                frase.append(risultatoApprossimativo[i].author);
                if(i<risultatoApprossimativo.size()-1) frase.append(",\n^1");
                else frase.append(".");
            }
        }
        block->tell(frase,numero);
        m_scheduler->addInstructionBlock( block, Server::LOW );
    }
}

void Analyzer::findOp(char* line)
{
    std::cout<<"[!] FindOp";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] FindOp";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numero;
    if (isAdminSay(line,numero))
    {
        InstructionsBlock * block = new InstructionsBlock();

        #ifdef ITA
            std::cout<<"  [OK] e' un admin. Eseguo il findop.\n";
            *(m_dati->log)<<"  [OK] e' un admin. Eseguo il findop.\n";
        #else
            std::cout<<"  [OK] Is an admin. Doing findop.\n";
            *(m_dati->log)<<"  [OK] Is an admin. Doing findop.\n";
        #endif


        //estraggo il nick da cercare.
        std::string temp(line);
        int pos=temp.find("!findop");
        std::string nick=temp.substr(pos+8);

        //ho il nick da cercare
        #ifdef ITA
            std::cout<<"  [-]Ricerca per "<<nick<<".\n";
            *(m_dati->log)<<"  [-]Ricerca per "<<nick<<".\n";
        #else
            std::cout<<"  [-]Searching for "<<nick<<".\n";
            *(m_dati->log)<<"  [-]Searching for "<<nick<<".\n";
        #endif
        //eseguo la ricerca sul DB e invio i risultati al server di gioco.
        std::vector<Db::idNickStruct> risultato=database->findPreciseIdNickViaNickOp(nick);
        std::vector<Db::idNickStruct> risultatoApprossimativo=database->findAproxIdNickViaNickOp(nick);

        #ifdef ITA
            std::cout<<"ricerca: "<<risultato.size()<<" "<<risultatoApprossimativo.size()<<"\n";
            std::string frase("^2Risultati esatti: \n^1");
        #else
            std::cout<<"search: "<<risultato.size()<<" "<<risultatoApprossimativo.size()<<"\n";
            std::string frase("^2Exact results: \n^1");
        #endif
        if (risultato.size()>5)
        {
            #ifdef ITA
                frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                frase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            frase.append(risultato[0].id);
            frase.append("^2:^1 ");
            frase.append(risultato[0].nick);
            frase.append(" ^2?");
        }
        else
        {
            if (risultato.empty()) frase.append("none.");
            else for (unsigned int i=0; i<risultato.size();i++)
            {
                frase.append(risultato[i].id);
                frase.append("^2:^1 ");
                frase.append(risultato[i].nick);
                if(i<risultato.size()-1) frase.append(",\n^1");
                else frase.append(".");
            }
        }
        block->tell( frase, numero );

        frase.clear();
        #ifdef ITA
            frase.append("^2Risultati ricerca: \n^1");
        #else
            frase.append("^2Search results: \n^1");
        #endif
        if (risultatoApprossimativo.size()>15)
        {
            #ifdef ITA
                frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                frase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            frase.append(risultatoApprossimativo[0].id);
            frase.append("^2:^1 ");
            frase.append(risultatoApprossimativo[0].nick);
            frase.append("\n^2, o forse \n^1");
            frase.append(risultatoApprossimativo[1].id);
            frase.append("^2:^1 ");
            frase.append(risultatoApprossimativo[1].nick);
            frase.append(" ^2?");
        }
        else
        {
            if (risultatoApprossimativo.empty()) frase.append("none.");
            else for (unsigned int i=0; i<risultatoApprossimativo.size();i++)
            {
                frase.append(risultatoApprossimativo[i].id);
                frase.append("^2:^1 ");
                frase.append(risultatoApprossimativo[i].nick);
                if(i<risultatoApprossimativo.size()-1) frase.append(",\n^1");
                else frase.append(".");
            }
        }
        block->tell(frase,numero);
        m_scheduler->addInstructionBlock( block, Server::LOW );
    }
}

void Analyzer::op(char* line)
{
    std::cout<<"[!] Op";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Op";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        int i=0;
        //prendo il numero o nome del player da aggiungere tra gli admin
        std::string temp(line);
        int pos=temp.find("!op");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+3);
        int end=temp.find_first_of(" \n",pos);
        std::string player=temp.substr(pos,end-pos);
        if (isA(line,_R_OP_NUMBER))
            i = findPlayer( player );
        else
            i = translatePlayer( player );
        if (i<0)
            #ifdef ITA
                block->tell("^1Errore: nick non univoco o giocatore non trovato.",numeroAdmin);
            #else
                block->tell("^1Error: nick not unique or player not found.",numeroAdmin);
            #endif
        else
        {
            if(!database->checkAuthGuid((*m_dati)[m_dati->serverNumber][i]->GUID) && database->addOp((*m_dati)[m_dati->serverNumber][i]->nick,(*m_dati)[m_dati->serverNumber][i]->GUID))
                #ifdef ITA
                    block->tell("^0BanBot: ^1admin aggiunto con successo.",numeroAdmin);
                #else
                    block->tell("^0BanBot: ^1admin successifully added.",numeroAdmin);
                #endif
            else
                #ifdef ITA
                    block->tell("^1Fail: player non aggiunto alla lista admin.",numeroAdmin);
                #else
                    block->tell("^1Fail: player not added to admin list.",numeroAdmin);
                #endif
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::deop(char* line)
{
    std::cout<<"[!] Deop";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Deop";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        //prendo i dati dell'utente e lo tolgo dagli op
        std::string temp(line);
        int pos=temp.find("!deop");
        pos=temp.find_first_of("0123456789",pos+5);
        int end=temp.find_first_of(" ",pos);
        std::string numero=temp.substr(pos,end-pos);

        //ho il numero, elimino dal database tutti i record relativi.
        std::string frase;
        if (database->deleteOp(numero))
            #ifdef ITA
                block->tell("^0BanBot: ^1utente tolto con successo dalla lista admin.",numeroAdmin);
            #else
                block->tell("^0BanBot: ^1user deleted sucessifully from admin list.",numeroAdmin);
            #endif
        else
            #ifdef ITA
                block->tell("^1^0BanBot ^1fail: è stato riscontrato un errore.",numeroAdmin);
            #else
                block->tell("^0BanBot ^1fail: player is still an admin.",numeroAdmin);
            #endif

        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::kick(char* line)
{
    std::cout<<"[!] Kick";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Kick";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!kick");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+5);
        int end=temp.find_first_of(" ",pos);
        std::string player=temp.substr(pos,end-pos);

        if (isA(line,_R_KICK_NUMBER))
        {
            #ifdef ITA
                std::string frase("^0BanBot: ^1butto fuori il player numero ");
            #else
                std::string frase("^0BanBot: ^1kicking player number ");
            #endif
            frase.append(player);
            frase.append("...");
            block->say(frase);
            block->kick(player);
        }
        else
        {
            int i=translatePlayer(player);
            if (i<0)
            {
                #ifdef ITA
                    block->tell("^1Errore: nick non trovato o non univoco.",numeroAdmin);
                #else
                    block->tell("^1Error: nick not unique or not found.",numeroAdmin);
                #endif
            }
            else
            {
                #ifdef ITA
                    std::string frase("^0BanBot: ^1butto fuori ");
                #else
                    std::string frase("^0BanBot: ^1kicking ");
                #endif
                frase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                frase.append("...");
                block->say(frase);
                block->kick((*m_dati)[m_dati->serverNumber][i]->number);
            }
        }
        m_scheduler->addInstructionBlock( block, Server::HIGH );
    }
}

void Analyzer::mute(char* line)
{
    std::cout<<"[!] Mute";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Mute";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        if ( isA(line,_R_MUTE_ALL) )
        {
            #ifdef ITA
            std::string frase("^0BanBot: ^1muto/smuto tutti.");
            #else
            std::string frase("^0BanBot: ^1muting/unmuting all players.");
            #endif
            block->say(frase);
            block->muteAll(numeroAdmin);
        }
        else
        {
            std::string temp(line);
            int pos=temp.find("!mute");
            pos=temp.find_first_not_of(" \t\n\r\f\v",pos+5);
            int end=temp.find_first_of(" ",pos);
            std::string player=temp.substr(pos,end-pos);

            if (isA(line,_R_MUTE_NUMBER))
            {
                #ifdef ITA
                    std::string frase("^0BanBot: ^1muto/smuto il giocatore numero ");
                #else
                    std::string frase("^0BanBot: ^1muting/unmuting player number ");
                #endif
                frase.append(player);
                frase.append("...");
                block->say(frase);
                block->mute(player);
            }
            else
            {
                int i=translatePlayer(player);
                if (i<0)
                {
                    #ifdef ITA
                        block->tell("^1Errore: nick non trovato o non univoco.",numeroAdmin);
                    #else
                        block->tell("^1Error: nick not unique or not found.",numeroAdmin);
                    #endif
                }
                else
                {
                    #ifdef ITA
                        std::string frase("^0BanBot: ^1muto/smuto ");
                    #else
                        std::string frase("^0BanBot: ^1muting/unmuting ");
                    #endif
                    frase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                    frase.append("...");
                    block->say(frase);
                    block->mute((*m_dati)[m_dati->serverNumber][i]->number);
                }
            }
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::help(char* line)
{
    std::cout<<"[!] Help";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Help";
    std::string numero;
    if (isAdminSay(line,numero))
    {
        InstructionsBlock * block = new InstructionsBlock();
        block->tell(COMMANDLIST,numero);
        m_scheduler->addInstructionBlock( block, Server::LOW );
    }
}

void Analyzer::setStrict(char* line)
{
    std::cout<<"[!] Strict";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Strict";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!strict");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+7);
        int end=temp.find_first_of(" ",pos);
        std::string variable=temp.substr(pos,end-pos);

        if ( variable.compare("off")==0 || variable.compare("OFF")==0 )
        {
            (*m_dati)[m_dati->serverNumber].setStrict(0);
            block->tell("^0BanBot: ^1Strict mode OFF.",numeroAdmin);

        }
        else
        {
            (*m_dati)[m_dati->serverNumber].setStrict( atoi(variable.c_str()) );
            #ifdef ITA
                std::string frase("^0BanBot: ^1Strict ora è al livello ");
            #else
                std::string frase("^0BanBot: ^1Strict mode now is on level ");
            #endif
            frase.append(variable);
            block->tell(frase,numeroAdmin);
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::veto(char* line)
{
    std::cout<<"[!] Veto";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Veto";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        block->veto();
        m_scheduler->addInstructionBlock( block, Server::HIGH );
    }
}

void Analyzer::slap(char* line)
{
    std::cout<<"[!] Slap";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Slap";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!slap");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+5);
        int end=temp.find_first_of(" ",pos);
        std::string player=temp.substr(pos,end-pos);
        int multiplier=1;
        std::string mul_string( "1" );
        if (isA(line,_R_SLAP_MORE))
        {
            pos=temp.find_first_of("2345",end);
            mul_string=temp.at(pos);
            multiplier=atoi(mul_string.c_str());
        }
        if (isA(line,_R_SLAP_NUMBER))
        {
            #ifdef ITA
                std::string frase("^0BanBot: ^1slappo il giocatore numero ");
            #else
                std::string frase("^0BanBot: ^1slapping player number ");
            #endif
            frase.append(player);
            frase.append(" ");
            frase.append(mul_string);
            #ifdef ITA
                frase.append(" volta(e)...");
            #else
                frase.append(" time(s)...");
            #endif
            block->say(frase);
            for (int i=0;i<multiplier;i++)
            {
                block->slap(player);
            }
        }
        else
        {
            int number=translatePlayer(player);
            if (number<0)
            {
                #ifdef ITA
                    block->tell("^1Errore: nick non trovato o non univoco.",numeroAdmin);
                #else
                    block->tell("^1Error: nick not unique or not found.",numeroAdmin);
                #endif
            }
            else
            {
                #ifdef ITA
                    std::string frase("^0BanBot: ^1slappo ");
                #else
                    std::string frase("^0BanBot: ^1slapping ");
                #endif
                frase.append((*m_dati)[m_dati->serverNumber][number]->nick);
                frase.append(" ");
                frase.append(mul_string);
                #ifdef ITA
                    frase.append(" volta(e)...");
                #else
                    frase.append(" time(s)...");
                #endif
                block->say(frase);
                for (int i=0;i<multiplier;i++)
                {
                    block->slap((*m_dati)[m_dati->serverNumber][number]->number);
                }
            }
        }
        m_scheduler->addInstructionBlock( block, Server::LOW );
    }
}

void Analyzer::nuke(char* line)
{
    std::cout<<"[!] Nuke";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Nuke";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!nuke");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+5);
        int end=temp.find_first_of(" ",pos);
        std::string player=temp.substr(pos,end-pos);

        if (isA(line,_R_NUKE_NUMBER))
        {
            #ifdef ITA
                std::string frase("^0BanBot: ^1nuke al giocatore numero ");
            #else
                std::string frase("^0BanBot: ^1nuking player number ");
            #endif
            frase.append(player);
            frase.append(".");
            block->say(frase);
            block->nuke(player);
        }
        else
        {
            int number=translatePlayer(player);
            if (number<0)
            {
                #ifdef ITA
                    block->tell("^1Errore: nick non trovato o non univoco.",numeroAdmin);
                #else
                    block->tell("^1Error: nick not unique or not found.",numeroAdmin);
                #endif
            }
            else
            {
                #ifdef ITA
                    std::string frase("^0BanBot: ^1nuke a ");
                #else
                    std::string frase("^0BanBot: ^1nuking ");
                #endif
                frase.append((*m_dati)[m_dati->serverNumber][number]->nick);
                frase.append(".");
                block->say(frase);
                block->nuke((*m_dati)[m_dati->serverNumber][number]->number);
            }
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}


void Analyzer::status(char* line)
{
    std::cout<<"[!] Status";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Status";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string frase( "^0BanBot ^1status: version " );
        frase.append( _VERSION );
        frase.append( ", coded by [2s2h]n3m3s1s & [2s2h]Zamy.\n^1Strict level: " );
        switch ((*m_dati)[m_dati->serverNumber].strict())
        {
            case 0:
                frase.append("OFF");
                break;
            case 1:
                frase.append("1");
                break;
            case 2:
                frase.append("2");
                break;
            case 3:
                frase.append("3");
                break;
            default:
                frase.append("4");
                break;
        }
        #ifdef ITA
            frase.append("\n^1Numbero degli admin: ");
            frase.append(database->ops());
            frase.append("\n^1Giocatori bannati: ");
            frase.append(database->banned());
            frase.append("\n^1Giocatori bannati automaticamente: ");
            frase.append(database->autoBanned());
        #else
            frase.append("\n^1Number of admins: ");
            frase.append(database->ops());
            frase.append("\n^1Players currently banned: ");
            frase.append(database->banned());
            frase.append("\n^1Players banned automatically: ");
            frase.append(database->autoBanned());
        #endif
        block->say(frase);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::force(char* line)
{
    std::cout<<"[!] Force";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Force";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!force");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+6);
        int end=temp.find_first_of(" ",pos);
        std::string action=temp.substr(pos,end-pos);
        pos=temp.find_first_not_of(" \t\n\r\f\v",end+1);
        std::string player=temp.substr(pos);
        if ( action.compare("spect") == 0 )
            action = "spectator";

        std::string frase;
        if (isA(line,_R_FORCE_NUMBER))
        {
            #ifdef ITA
                frase.append("^0BanBot: ^1sposto il giocatore numero ");
                frase.append(player);
                frase.append(" in ");
            #else
                frase.append("^0BanBot: ^1forcing player number ");
                frase.append(player);
                frase.append(" to ");
            #endif
            frase.append(action);
            frase.append(".");
        }
        else
        {
            int i=translatePlayer(player);
            if (i<0)
            {
                #ifdef ITA
                    block->tell("^1Errore: nick non trovato o non univoco.",numeroAdmin);
                #else
                    block->tell("^1Error: nick not unique or not found.",numeroAdmin);
                #endif
            }
            else
            {
                #ifdef ITA
                    frase.append("^0BanBot: ^1sposto ");
                    frase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                    frase.append(" in ");
                #else
                    frase.append("^0BanBot: ^1forcing ");
                    frase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                    frase.append(" to ");
                #endif
                frase.append(action);
                frase.append(".");
                player=(*m_dati)[m_dati->serverNumber][i]->number;
            }
        }
        block->say(frase);
        block->force(player,action);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::iamgod(char* line)
{
    std::cout<<"[!] iamgod";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] iamgod";
    //controllo se il database è vuoto. Se lo è, aggiungo la persona tra gli op.
    if (database->isOpTableEmpty())   //se il database è vuoto
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp = line;
        int pos = temp.find( "say:" );
        pos = temp.find_first_not_of( " ", pos+4 );
        int end = temp.find_first_of( " ", pos );
        std::string numero = temp.substr( pos, end-pos );
        //prendo i dati dell'utente e lo aggiungo tra gli op
        int i = findPlayer( numero );
        if( i>=0 && database->addOp( correggi((*m_dati)[m_dati->serverNumber][i]->nick), correggi((*m_dati)[m_dati->serverNumber][i]->GUID)) )
            block->bigtext("^1Welcome, my Master!");
        else
            #ifdef ITA
                block->tell("^1Fail: player non aggiunto alla lista admin.",numero);
            #else
                block->tell("^1Fail: player not added to admin list.",numero);
            #endif
        #ifdef DEBUG_MODE
            std::cout << "Player number: " << i << "\n";
        #endif

        m_scheduler->addInstructionBlock( block, Server::HIGH );
    }
}

void Analyzer::map(char* line)
{
    std::cout<<"[!] Map";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Map";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        //prendo il numero o nome del player da aggiungere tra gli admin
        std::string temp(line);
        int pos=temp.find("!map");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+4);
        int end=temp.find_first_of(" \n",pos);
        std::string mappa=temp.substr(pos,end-pos);
        block->map(mappa);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::nextmap(char* line)
{
    std::cout<<"[!] NextMap";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] NextMap";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        //prendo il numero o nome del player da aggiungere tra gli admin
        std::string temp(line);
        int pos=temp.find("!nextmap");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+8);
        int end=temp.find_first_of(" \n",pos);
        std::string mappa=temp.substr(pos,end-pos);
        block->nextmap(mappa);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::admins(char* line)
{
    std::cout<<"[!] Admins";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Admins";
    //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::vector<unsigned int> t=admins();
        std::string mex("^1Admins:\n");
        for (unsigned int i=0;i<t.size();i++)
        {
            mex.append( "^1" );
            mex.append( (*m_dati)[m_dati->serverNumber][t[i]]->nick );
            #ifdef ITA
                mex.append( "^2 e' " );
            #else
                mex.append( "^2 is " );
            #endif
            mex.append( database->adminRegisteredNickViaGuid( (*m_dati)[m_dati->serverNumber][t[i]]->GUID ) );
            if (i>0 && i%2!=0)
                mex.append("\n");
            else
                mex.append(" ");
        }
        block->tell(mex,numeroAdmin);
        m_scheduler->addInstructionBlock( block, Server::LOW );
    }
}

void Analyzer::pass(char* line)
{
    std::cout<<"[!] g_Password";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] g_Password";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock* block = new InstructionsBlock();
        std::string temp = line;
        int pos = temp.find( "!pass" );
        pos = temp.find_first_not_of( " ", pos+5 );
        int end = temp.find_first_of( " ", pos );
        std::string password = temp.substr( pos, end-pos );

        #ifdef ITA
            std::string phrase ("^0BanBot:^1 password cambiata in ^2");
        #else
            std::string phrase ("^0BanBot:^1 password changed to ^2");
        #endif
        phrase.append( password );
        block->changePassword( password );
        block->tell(phrase,numeroAdmin);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }

}

void Analyzer::config(char* line)
{
    std::cout<<"[!] Config(exec)";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Config(exec)";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin))
    {
        InstructionsBlock *block = new InstructionsBlock();
        std::string temp = line;
        int pos = temp.find( "!config" );
        pos = temp.find_first_not_of( " ", pos+7 );
        int end = temp.find_first_of( " ", pos );
        std::string file = temp.substr( pos, end-pos );

        #ifdef ITA
        std::string phrase ("^0BanBot:^1 carico il file di configurazione ^2");
        #else
        std::string phrase ("^0BanBot:^1 loading the configuration file ^2");
        #endif

        phrase.append( file );
        block->say(phrase);
        block->exec( file );
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

/*************************************************************************** UTILS **************************************/

void Analyzer::getDateAndTime(std::string &data,std::string &ora)
{
    time_t tempo;
    tempo=time(NULL);
    struct tm *tmp;
    tmp = localtime(&tempo);
    char outstr[20];
    strftime(outstr, sizeof(outstr), "%H:%M", tmp);
    ora=outstr;

    strftime(outstr, sizeof(outstr), "%F", tmp);
    data=outstr;
}

void Analyzer::buttaFuori(const std::string &reason, const std::string numero, const std::string nick)
{
    InstructionsBlock * block = new InstructionsBlock();
    //è stato bannato, lo butto fuori
    #ifdef ITA
        std::cout<<"  [+] kick per ban.\n";
        *(m_dati->log)<<"  [+] kick per ban.\n";
        std::string frase( "^0BanBot: ^1kick per il giocatore numero " );
        frase.append(numero);
        frase.append( ", " );
        frase.append( nick );
        if ( !reason.empty() )
        {
            frase.append( " bannato per " );
            frase.append( reason );
        }
    #else
        std::cout<<"  [+] kick due to ban.\n";
        *(m_dati->log)<<"  [+] kick due to ban.\n";
        std::string frase( "^0BanBot: ^1kicking player number " );
        frase.append(numero);
        frase.append( ", " );
        frase.append( nick );
        if ( !reason.empty() )
        {
            frase.append( " banned for " );
            frase.append( reason );
        }
    #endif
    frase.append( "." );
    block->say( frase );
    block->kick( numero );
    m_scheduler->addInstructionBlock( block , Server::HIGH );
}

bool Analyzer::guidIsBanned(const std::string &guid, const std::string &nick, const std::string &numero, const std::string &ip)
{
    //if ( (*m_dati)[m_dati->serverNumber].strict() > LEVEL0 )
    //{
        std::vector<Db::idMotiveStruct> risultato = database->idMotiveViaGuid(correggi(guid));

        if ( risultato.size() )
        {
            //butto fuori la persona dal server
            #ifdef ITA
                std::cout<<"  [!] kick a "<<nick<<" per ban ("<<risultato[0].motive<<").\n";
                *(m_dati->log)<<"  [!] kick a "<<nick<<" per ban ("<<risultato[0].motive<<").\n";
            #else
                std::cout<<"  [!] kicking "<<nick<<" for ban ("<<risultato[0].motive<<").\n";
                *(m_dati->log)<<"  [!] kicking "<<nick<<" for ban ("<<risultato[0].motive<<").\n";
            #endif
            buttaFuori(risultato[0].motive,numero,nick);

            //aggiorno i dati sul database
            std::string ora;
            std::string data;
            getDateAndTime(data,ora);
            database->modifyBanned(correggi(nick),ip,data,ora,std::string(),risultato[0].id);
            return true;
        }
    //}
    return false;
}

bool Analyzer::nickIsBanned(const std::string &nick, const std::string &numero, const std::string &ip, const std::string &guid)
{
    bool bannato = false;

    //if ( (*m_dati)[m_dati->serverNumber].strict() > LEVEL0 )
    //{
        std::vector<Db::idMotiveStruct> risultato = database->idMotiveViaNick(correggi(nick));
        std::string ora;
        std::string data;
        getDateAndTime(data,ora);

        if ( risultato.size() )
        {
            //se sono in modalità strict di livello 2, se il nick è bannato, butto fuori anche se è passata un'ora.
            if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL2 ) {bannato = true;}
            else
            {
                //sono in modalita' strict di livello 1. Se è bannato da meno di un'ora, lo butto fuori, altrimenti avviso gli admin
                int oraAttuale=atoi(ora.substr(0,2).c_str());
                int minutoAttuale=atoi(ora.substr(3,5).c_str());
                int oraBan = atoi(risultato[0].time.substr(0,2).c_str());
                int minutoBan = atoi(risultato[0].time.substr(3,5).c_str());
                int diff = (oraAttuale*60+minutoAttuale)-(oraBan*60+minutoBan);
                if ( data.compare(risultato[0].date) == 0 && diff>0 && diff<60 )
                {
                    bannato = true;
                }
                else
                {
                    #ifdef ITA
                        std::string frase("^0BanBot ^1attenzione: il nick '");
                        frase.append(nick);
                        frase.append("' e' tra quelli bannati.");
                    #else
                        std::string frase("^0BanBot ^1warning: the nick '");
                        frase.append(nick);
                        frase.append("' is currently banned.");
                    #endif
                    tellToAdmins(frase);
                }
            }
            if ( bannato )
            {
                #ifdef ITA
                    std::cout<<"  [!] kick a "<<nick<<" per ban ("<<risultato[0].motive<<").\n";
                    *(m_dati->log)<<"  [!] kick a "<<nick<<" per ban ("<<risultato[0].motive<<").\n";
                #else
                    std::cout<<"  [!] kicking "<<nick<<" for ban ("<<risultato[0].motive<<").\n";
                    *(m_dati->log)<<"  [!] kicking "<<nick<<" for ban ("<<risultato[0].motive<<").\n";
                #endif
                buttaFuori(risultato[0].motive, numero, nick);

                std::string ora;
                std::string data;
                getDateAndTime(data,ora);
                database->modifyBanned(std::string(),ip,data,ora,std::string(),risultato[0].id);
                database->insertNewGuid(correggi(guid),risultato[0].id);
            }
        }
    //}
    return bannato;
}

bool Analyzer::ipIsBanned(const std::string &ip, const std::string &numero, const std::string &nick, const std::string &guid)
{

    //if ( (*m_dati)[m_dati->serverNumber].strict() > LEVEL0 )
    //{
        std::vector<Db::idMotiveStruct> risultato = database->idMotiveViaIp( ip );

        if ( risultato.size() )
        {
            std::string ora;
            std::string data;
            getDateAndTime(data,ora);

            int oraAttuale=atoi(ora.substr(0,2).c_str());
            int minutoAttuale=atoi(ora.substr(3,5).c_str());
            int oraBan=atoi(risultato[0].time.substr(0,2).c_str());
            int minutoBan=atoi(risultato[0].time.substr(3,5).c_str());
            int diff=(oraAttuale*60+minutoAttuale)-(oraBan*60+minutoBan);
            if (data.compare(risultato[0].date)==0 && diff>0 && diff<60)
            {
                #ifdef ITA
                    std::cout<<"  [!] kick a "<<nick<<" per ban ("<<risultato[0].motive<<").\n";
                    *(m_dati->log)<<"  [!] kick a "<<nick<<" per ban ("<<risultato[0].motive<<").\n";
                #else
                    std::cout<<"  [!] kicking "<<nick<<" for ban ("<<risultato[0].motive<<").\n";
                    *(m_dati->log)<<"  [!] kicking "<<nick<<" for ban ("<<risultato[0].motive<<").\n";
                #endif
                buttaFuori(risultato[0].motive, numero,nick);

                database->modifyBanned(correggi(nick),std::string(),data,ora,std::string(),risultato[0].id);
                database->insertNewGuid(correggi(guid),risultato[0].id);
                return true;
            }
        }
    //}
    return false;
}

std::string Analyzer::correggi(std::string stringa)
{
    unsigned int pos=0;
    bool nonFinito=true;
    while (nonFinito)
    {
        nonFinito=false;
        pos=stringa.find("'",pos);
        if (pos<stringa.size())
        {
            stringa=stringa.replace(pos,1,"''");
            pos+=2;
            nonFinito=true;
        }
    }
    return stringa;
}

std::vector<unsigned int> Analyzer::admins()
{
    std::vector<unsigned int> temp;
    for (unsigned int i=0;i<(*m_dati)[m_dati->serverNumber].size();i++)
    {
        if (database->checkAuthGuid(correggi((*m_dati)[m_dati->serverNumber][i]->GUID)))
            temp.push_back(i);
    }
    return temp;
}

void Analyzer::tellToAdmins(std::string frase)
{
    InstructionsBlock * block = new InstructionsBlock();
    std::vector<unsigned int> indici=admins();
    for (unsigned int i=0;i<indici.size();i++)
    {
        block->tell(frase,(*m_dati)[m_dati->serverNumber][indici[i]]->number);
    }
    m_scheduler->addInstructionBlock( block , Server::LOW );
}

int Analyzer::translatePlayer(std::string player)
{
    bool unique=true;
    int index=-1;
    for (unsigned int i=0;i<(*m_dati)[m_dati->serverNumber].size();i++)
    {
        if ((*m_dati)[m_dati->serverNumber][i]->nick.find(player)<(*m_dati)[m_dati->serverNumber][i]->nick.size())
        {
            //il giocatore corrisponde
            if (index<0) index=i;
            else unique=false;
        }
    }
    if (!unique) index=-1;
    return index;
}

int Analyzer::findPlayer( std::string number )
{
    bool nonTrovato = true;
    int index = -1;

    for (unsigned int i=0; nonTrovato &&  (i<(*m_dati)[m_dati->serverNumber].size()); i++)
    {
        if ((*m_dati)[m_dati->serverNumber][i]->number.compare(number)==0)
        {
            index=i;
            nonTrovato=false;
        }
    }

    return index;
}


//main loop
void Analyzer::main_loop()
{
    InstructionsBlock * block = new InstructionsBlock();
    #ifdef ITA
        std::cout<<"[OK] BanBot avviato.\n\n";
        *(m_dati->errors)<<"[OK] BanBot avviato.\n\n";
    #else
        std::cout<<"[OK] BanBot launched.\n\n";
        *(m_dati->errors)<<"[OK] BanBot launched.\n\n";
    #endif
    while (true)
    {
        commandexecuted=false;
        //vedo è il caso di fare il backup
        if(backup->doJobs())
        {
            //eseguito il backup. Reload dei server
            block->reload();//block->reload(true);
            m_scheduler->addInstructionBlock( block, Server::MEDIUM );
            sleep(4);
            //e azzero anche la linea dove ero arrivato, se ha fatto il backup del file
            for (unsigned int i=0;i<m_dati->size();i++)
            {
                log=new ifstream();
                log->open((*m_dati)[i].botLog().c_str());
                if (log->is_open())
                {
                    //se il file è aperto, controllo la dimensione
                    log->seekg (0, ios:: end);
                    if (log->tellg ()<(*m_dati)[i].row()) (*m_dati)[i].setRow(0);
                }
                //altrimenti se non c'è il file, sono già sicuro che il backup è stato fatto.
                else (*m_dati)[i].setRow(0);
                log->close();
                delete log;
                for (unsigned int j=0;j<(*m_dati)[i].size();j++) delete (*m_dati)[i][j];
                //resetto il vector:
                (*m_dati)[i].clear();
                m_dati->serverNumber=0;
            }
            sleep(2);
        }

        //ogni tot di giri controllo se sono cambiate le impostazioni
        if ( giri > 10 )
        {
            giri = 0;
            //test sulle impostazioni
            if ( m_configLoader->testChanges() )
            {
                m_configLoader->loadOptions();
                loadOptions();
            }
        }

        //inzio il ciclo per gestire i server
        for (m_dati->serverNumber=0;m_dati->serverNumber<m_dati->size();m_dati->serverNumber++)
        {
            if ( (*m_dati)[m_dati->serverNumber].isValid() )
            {
                //provo ad aprire il file e a riprendere dalla riga dove ero arrivato
                (m_dati->log)->changePath( (*m_dati)[m_dati->serverNumber].botLog() );
                #ifdef ITA
                    std::cout<<"Provo ad aprire "<<(*m_dati)[m_dati->serverNumber].serverLog()<<"\n";
                #else
                    std::cout<<"Trying to open "<<(*m_dati)[m_dati->serverNumber].serverLog()<<"\n";
                #endif
                log=new ifstream();
                log->open((*m_dati)[m_dati->serverNumber].serverLog().c_str());
                log->seekg((*m_dati)[m_dati->serverNumber].row());
                if (!log->is_open())
                {
                    #ifdef ITA
                        std::cout<<"  [FAIL] Non sono riuscito ad aprirlo!\n";
                        *(m_dati->log)<<"  [FAIL] Non sono riuscito ad aprire "<<(*m_dati)[m_dati->serverNumber].serverLog()<<"\n";
                        *(m_dati->errors)<<"[FAIL] In " << (*m_dati)[m_dati->serverNumber].name() << ": non sono riuscito ad aprire "<<(*m_dati)[m_dati->serverNumber].serverLog()<<"\n";
                    #else
                        std::cout<<"  [FAIL] I'm not able to open it!\n";
                        *(m_dati->log)<<"  [FAIL] I'm not able to open "<<(*m_dati)[m_dati->serverNumber].serverLog()<<"\n";
                        *(m_dati->errors)<<"[FAIL] On " << (*m_dati)[m_dati->serverNumber].name() << ": I'm not able to open "<<(*m_dati)[m_dati->serverNumber].serverLog()<<"\n";
                    #endif
                }
                //se il file è aperto posso lavorare, e provo ad aprire pure il db
                else if (database->openDatabase())
                {
                    #ifdef ITA
                        std::cout<<"  [OK] Aperto!\n";;
                        #ifdef DEBUG_MODE
                        std::cout<< "  Al punto: "<< (*m_dati)[m_dati->serverNumber].row()<<"\n";
                        *(m_dati->log)<<"  [OK] Aperto!\n";
                        #endif
                    #else
                        std::cout<<"  [OK] Opened!\n";;
                        #ifdef DEBUG_MODE
                        std::cout<< "  At: "<< (*m_dati)[m_dati->serverNumber].row()<<"\n";
                        *(m_dati->log)<<"  [OK] Opened!\n";
                        #endif
                    #endif
                    //il file è aperto, esamino le nuove righe (se ce ne sono)
                    while (!log->eof() && !log->bad() && (*m_dati)[m_dati->serverNumber].row()>=0)
                    {
                        //leggo una riga
                        char line [1500];
                        log->getline(line,1500,'\n');
                        //se non è la fine del file, mi salvo la riga dove sono arrivato
                        if (!log->eof() && !log->bad()) (*m_dati)[m_dati->serverNumber].setRow(log->tellg());

                        #ifdef DEBUG_MODE
                            std::cout<< "  At: "<< (*m_dati)[m_dati->serverNumber].row()<<" contains: "<<line<<"\n";
                            *(m_dati->log)<< "  At: "<< (*m_dati)[m_dati->serverNumber].row()<<" contains: "<<line<<"\n";
                        #endif

                        //comincio coi test
                        if (isA(line, _R_CLIENT_USER_INFO))
                        {
                            //ha passato il regex, è una clientUserinfo
                            clientUserInfo(line);
                            commandexecuted=true;
                        }
                        else
                        {
                            //non ha passato il test, non è un clientUserinfo: provo con gli altri regex
                            //controllo se è la connessione di un utente
                            if (isA(line, _R_CLIENT_CONNECT))
                            {
                                //è un clientconnect:
                                clientConnect(line);
                                commandexecuted=true;
                            }
                            else
                            {
                                //non è un clientConnect, provo con gli altri regex_t
                                //controllo se è una disconnessione
                                if (isA(line, _R_CLIENT_DISCONNECT))
                                {
                                    //è un clientDisconnect
                                    clientDisconnect(line);
                                }
                                else
                                {
                                    //non è neanche un clientDisconnect
                                    if (isA(line, _R_INITGAME))
                                    {
                                        //ok, è l'inizio di una nuova partita, resetto i player:
                                        //elimino gli oggetti Player:
                                        for (unsigned int i=0;i<(*m_dati)[m_dati->serverNumber].size();i++) delete (*m_dati)[m_dati->serverNumber][i];
                                        //resetto il vector:
                                        (*m_dati)[m_dati->serverNumber].clear();
                                        commandexecuted=true;
                                    }
                                    else
                                    {
                                        if (isA(line,_R_COMMAND))
                                        {
                                            commandexecuted=true;
                                            //controllo se è un comando di ban
                                            if (isA(line, _R_BAN))
                                            {
                                                //è una richiesta di ban:
                                                ban(line);
                                            }
                                            else
                                            {
                                                //controllo se è la richiesta di un find
                                                if (isA(line,_R_FIND))
                                                {
                                                    //è un find.
                                                    find(line);
                                                }
                                                else
                                                {
                                                    //controllo se è una richiesta di unban
                                                    if (isA(line, _R_UNBAN))
                                                    {
                                                        //è un comando di unban
                                                        unban(line);
                                                    }
                                                    else
                                                    {
                                                        if (isA(line, _R_OP))
                                                        {
                                                            //è un comando di op
                                                            op(line);
                                                        }
                                                        else
                                                        {
                                                            if (isA(line, _R_DEOP))
                                                            {
                                                                //è un comando di deop
                                                                deop(line);
                                                            }
                                                            else
                                                            {
                                                                if (isA(line, _R_HELP))
                                                                {
                                                                    //è un help
                                                                    help(line);
                                                                }
                                                                else
                                                                {
                                                                    if (isA(line, _R_FINDOP))
                                                                    {
                                                                        //è un findop
                                                                        findOp(line);
                                                                    }
                                                                    else
                                                                    {
                                                                        if (isA(line, _R_KICK))
                                                                        {
                                                                            //è un kick
                                                                            kick(line);
                                                                        }
                                                                        else
                                                                        {
                                                                            if (isA(line, _R_MUTE))
                                                                            {
                                                                                //è un mute
                                                                                mute(line);
                                                                            }
                                                                            else
                                                                            {
                                                                                if (isA(line, _R_STRICT))
                                                                                {
                                                                                    //è uno strict
                                                                                    setStrict(line);
                                                                                }
                                                                                else
                                                                                {
                                                                                    if (isA(line, _R_VETO))
                                                                                    {
                                                                                        //è un veto
                                                                                        veto(line);
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        if (isA(line, _R_SLAP))
                                                                                        {
                                                                                            //è uno slap
                                                                                            slap(line);
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            if (isA(line, _R_STATUS))
                                                                                            {
                                                                                                //è uno status
                                                                                                status(line);
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                if (isA(line, _R_NEXTMAP))
                                                                                                {
                                                                                                    //è un nextmap
                                                                                                    nextmap(line);
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    if (isA(line, _R_MAP))
                                                                                                    {
                                                                                                        //è un map
                                                                                                        map(line);
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        if (isA(line, _R_FORCE))
                                                                                                        {
                                                                                                            //è un force
                                                                                                            force(line);
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                            if (isA(line, _R_NUKE))
                                                                                                            {
                                                                                                                //è un nuke
                                                                                                                nuke(line);
                                                                                                            }
                                                                                                            else
                                                                                                            {
                                                                                                                if (isA(line, _R_ADMINS))
                                                                                                                {
                                                                                                                    //è un admins
                                                                                                                    admins(line);
                                                                                                                }
                                                                                                                else
                                                                                                                {
                                                                                                                    if (isA(line, _R_PASS))
                                                                                                                    {
                                                                                                                        //è un password
                                                                                                                        pass(line);
                                                                                                                    }
                                                                                                                    else
                                                                                                                    {
                                                                                                                        if (isA(line, _R_CONFIG))
                                                                                                                        {
                                                                                                                            //è un config
                                                                                                                            config(line);
                                                                                                                        }
                                                                                                                        else
                                                                                                                        {
                                                                                                                            if (isA(line, _R_IAMGOD))
                                                                                                                            {
                                                                                                                                //è un iamgod
                                                                                                                                iamgod(line);
                                                                                                                            }
                                                                                                                            else
                                                                                                                            {
                                                                                                                                expansion(line);
                                                                                                                            }
                                                                                                                        }
                                                                                                                    }
                                                                                                                }
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    #ifdef ITA
                        (*m_dati)[m_dati->serverNumber].setRow(0);//se non riesco ad aprire il file, ricomincio dalla prima riga
                        std::cout<<"   [FAIL] Non riesco ad aprire il database\n";
                        (m_dati->log)->timestamp();
                        *(m_dati->log)<<"   [FAIL] Non riesco ad aprire il database\n";
                        (m_dati->errors)->timestamp();
                        *(m_dati->errors)<<"[FAIL] Non riesco ad aprire il database del server "<< (*m_dati)[m_dati->serverNumber].name()  <<"\n";
                    #else
                        (*m_dati)[m_dati->serverNumber].setRow(0);//se non riesco ad aprire il file, ricomincio dalla prima riga
                        std::cout<<"   [FAIL] Can't open database\n";
                        (m_dati->log)->timestamp();
                        *(m_dati->log)<<"   [FAIL] Can't open database\n";
                        (m_dati->errors)->timestamp();
                        *(m_dati->errors)<<"[FAIL] Can't open database of server "<< (*m_dati)[m_dati->serverNumber].name()  <<"\n";
                    #endif
                }
                if ((*m_dati)[m_dati->serverNumber].row()<0) (*m_dati)[m_dati->serverNumber].setRow(0);
                //chiudo il file di log del server
                log->close();
                delete log;
                database->closeDatabase();
                (m_dati->log)->close();
                (m_dati->errors)->close();
            }
        }
        giri++;
        //fine ciclo, lascio passare un po' di tempo
        if ( commandexecuted )
        {
            fascia=0;
            contatore=0;
        }
        else contatore++;
        if ( (fascia == 0 && contatore > 15) || (fascia == 1 && contatore > 60) )
        {
            fascia++;
            contatore++;
        }

        switch (fascia)
        {
            case 0:
                sleep(TIME_SLEEPING_MIN);
                break;
            case 1:
                sleep(TIME_SLEEPING_MIDDLE);
                break;
            case 2:
                sleep(TIME_SLEEPING_MAX);
                break;
            default:
                sleep(TIME_SLEEPING_MIN);
                break;
        }
    }
}
#endif