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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


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

//regex defines:
#define _R_NUMBER "^[0-9]+$"

#define _R_CLIENT_CONNECT " *[0-9]+:[0-9]{2} *ClientConnect:"
#define _R_CLIENT_USER_INFO " *[0-9]+:[0-9]{2} *ClientUserinfo:"
#define _R_CLIENT_USER_INFO_CHANGED " *[0-9]+:[0-9]{2} *ClientUserinfoChanged:"
#define _R_COMPLETE_CLIENT_USER_INFO " *[0-9]+:[0-9]{2} *ClientUserinfo: +[0-9]+ +\\ip\\[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}:[0-9]{1,6}\\name\\[^ \t\n\r\f\v]+\\racered\
\\[0-9]{1}\\raceblue\\[0-9]{1}\\rate\\[0-9]+\\ut_timenudge\\[0-9]+\\cg_rgb\\[0-9]{1,3} [0-9]{1,3} [0-9]{1,3}\\cg_predictitems\\[01]{1}\\cg_physics\\[01]{1}\\snaps\\[0-9]{1,2}\\model\\[^ \t\n\r\f\v]+\
\\headmodel\\[^ \t\n\r\f\v]+\\team_model\\[^ \t\n\r\f\v]+\\team_headmodel\\[^ \t\n\r\f\v]+\\color1\\[0-9]{1,2}\\color2\\[0-9]{1,2}\\handicap\\100\\sex\\[^ \t\n\r\f\v]+\\cl_anonymous\\[01]{1}\
\\gear\\[^ \t\n\r\f\v]+\\teamtask\\[0-9]+\\cl_guid\\[A-F0-9]{32}\\weapmodes\\[0-2]{20}"

#define _R_CLIENT_DISCONNECT "^ *[0-9]+:[0-9]{2} *ClientDisconnect:"
#define _R_BAN "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!ban [^\t\n\r\f\v]+"
#define _R_BAN_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!ban [0-9]{1,2}"
#define _R_FIND "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!find [^ \t\n\r\f\v]+$"
#define _R_FINDOP "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!findop [^ \t\n\r\f\v]+$"
#define _R_UNBAN "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!unban [^ \t\n\r\f\v]+$"
#define _R_UNBAN_ID "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!unban [0-9]+$"
#define _R_OP "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!op [^ \t\n\r\f\v]+"
#define _R_OP_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!op [0-9]{1,2}"
#define _R_DEOP "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!deop [^ \t\n\r\f\v]+$"
#define _R_DEOP_ID "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!deop [0-9]+$"
#define _R_GUID "[A-F0-9]{32}"
#define _R_INITGAME "^ *[0-9]+:[0-9]{2} *InitGame:"
#define _R_HELP "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!help$"
#define _R_KICK "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!kick [^ \t\n\r\f\v]+$"
#define _R_KICK_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!kick [0-9]{1,2}$"
#define _R_MUTE " *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!mute [^ \t\n\r\f\v]+$"
#define _R_MUTE_ALL "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!mute (all|ALL)$"
#define _R_MUTE_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!mute [0-9]{1,2}$"
#define _R_STRICT "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!strict (OFF|off|[0-5]{1})$"
#define _R_VETO "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!veto$"
#define _R_SLAP "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!slap [^\t\n\r\f\v]+$"
#define _R_SLAP_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!slap [0-9]{1,2}"
#define _R_SLAP_MORE "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!slap [^ \t\n\r\f\v]+ [2-9]{1}$"
#define _R_NUKE "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!nuke [^ \t\n\r\f\v]+$"
#define _R_NUKE_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!nuke [0-9]{1,2}$"
#define _R_COMMAND "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +![^\t\n\r\f\v]+$"
#define _R_STATUS "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!status$"
#define _R_FORCE "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!force (red|blue|spect) [^ \t\n\r\f\v]+$"
#define _R_FORCE_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!force (red|blue|spect) [0-9]{1,2}$"
#define _R_IAMGOD "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!iamgod$"
#define _R_MAP "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!map [^\t\n\r\f\v]+$"
#define _R_NEXTMAP "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!nextmap [^\t\n\r\f\v]+$"
#define _R_ADMINS "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!admins$"
#define _R_EMPTY_PASS "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!pass"
#define _R_PASS "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!pass [^\t\n\r\f\v]+$"
#define _R_CONFIG "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!config [^\t\n\r\f\v]+$"
#define _R_WARNINGS "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!warnings (off|public|private)$"
#define _R_BANTIMEWARN "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!bantimewarn (on|off)$"
#define _R_RESTART "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!restart$"
#define _R_RELOAD "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!reload$"
#define _R_BALANCE "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!balance$"
#define _R_GRAVITY "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!grav ([0-9]{1,4}|off)$"
#define _R_CHANGELEVEL "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!level [^ \t\n\r\f\v]+ [0-9]{1,2}$"
#define _R_CHANGELEVEL_NUMBER "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!level [0-9]+ [0-9]{1,2}$"
#define _R_BIGTEXT "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!bigtext [^\t\n\r\f\v]+$"
#define _R_TEAMS "^ *[0-9]+:[0-9]{2} *say: +[0-9]+ +[^ \t\n\r\f\v]+: +!teams$"

//costruttore
Analyzer::Analyzer(Connection* conn, Db* db, ConfigLoader* configLoader )
    : backup( new Backup( configLoader->getOptions(), db ) )
    , m_configLoader( configLoader )
    , log(new std::ifstream())
    , contatore(0)
    , fascia(0)
    , giri(0)
    , server( conn )
    , database( db )
    , m_dati( configLoader->getOptions() )
    , m_scheduler( new Scheduler( m_dati , server ) )
    , m_fileLister( new FileLister(m_dati) )
{
    loadOptions();

    #ifdef ITA
        std::cout<<"[OK] Analyzer inizializzato.\n";
        *(m_dati->errors)<<"[OK] Analyzer inizializzato.\n\n";
    #else
        std::cout<<"\e[0;32m[OK] Analyzer initialized. \e[0m \n";
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
            std::ifstream * temp=new std::ifstream();
            temp->open((*m_dati)[i].serverLog().c_str());
            if ( temp->is_open() )
            {
                temp->seekg ( 0, std::ios::end );
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
    
    std::cout<<m_dati->toString()<<"\n";
    *(m_dati->errors)<<m_dati->toString()<<"\n";
    
    m_fileLister->updateServerConfigMapList();
    
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

//check a line with a regex (regular expression): if the line respect the rule, returns true
bool Analyzer::isA(const char* line, const std::string& regex )
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

int Analyzer::isAdminSay( char* line, std::string &numero )
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
        return database->checkAuthGuid( correggi((*m_dati)[m_dati->serverNumber][i]->GUID) );
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
    return 404;
}

void Analyzer::clientUserInfoChanged(char* line)
{
    // prendo il numero giocatore e la guid, utilizzando le funzioni delle stringhe
    std::string temp=line;
    int pos=temp.find("ClientUserinfoChanged:");
    pos=temp.find_first_not_of(' ',pos+22);
    int end=temp.find_first_of(' ',pos);
    std::string numero=temp.substr(pos,end-pos);
    pos=temp.find(" n");
    pos=temp.find_first_not_of("\\",pos+2);
    end=temp.find_first_of("\\ ",pos);
    std::string nick=temp.substr(pos,end-pos);
    pos=temp.find("\\t\\");
    pos=temp.find_first_not_of("\\",pos+2);
    end=temp.find_first_of("\\",pos);        //permetto anche spazi all'interno del nome, tutti i caratteri permessi tranne lo slash
    std::string team=temp.substr(pos,end-pos);
    
    #ifdef ITA
    std::cout<<"[-]Estrapolati i dati: numero="<<numero<<" nick="<<nick<<" team="<<team<<"\n";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[-]Estrapolati i dati: numero="<<numero<<" nick="<<nick<<" team="<<team<<"\n";
    #else
    std::cout<<"[-]Extracted data: number="<<numero<<" nick="<<nick<<" team="<<team<<"\n";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[-]Extracted data: number="<<numero<<" nick="<<nick<<" team="<<team<<"\n";
    #endif
    
    int i = findPlayer( numero );
    if( i >= 0 ){
        (*m_dati->currentServer())[i]->nick = nick;
        if( team.compare("1") == 0 ){
            (*m_dati->currentServer())[i]->team = Server::Player::RED;
        }
        else if( team.compare("2") == 0 ){
            (*m_dati->currentServer())[i]->team = Server::Player::BLUE;
        }
        else
            (*m_dati->currentServer())[i]->team = Server::Player::SPECT;
    }
}

void Analyzer::clientUserInfo(char* line)
{
    InstructionsBlock * block = new InstructionsBlock();    // create block for instructions
    // take player's info
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
            if (!guid.empty() && (*m_dati)[m_dati->serverNumber].strict() >= LEVEL2)
            {
                // illegal change of GUID => cheats
                if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL4 )
                {
                    kicked=true;

                    #ifdef ITA
                        std::cout<<"  [!] ban automatico per cheats (GUID cambiata durante il gioco).\n";
                        *(m_dati->log)<<"  [!] ban automatico per cheats (GUID cambiata durante il gioco).\n";
                    #else
                        std::cout<<"  [!] automated ban for cheats (GUID changed during the game).\n";
                        *(m_dati->log)<<"  [!] automated ban for cheats (GUID changed during the game).\n";
                    #endif
                    std::string phrase("^0BanBot: ^1auto-banning player number ");
                    phrase.append(numero);
                    phrase.append(", ");
                    phrase.append(nick);
                    phrase.append("^1 for cheats.");
                    block->say(phrase);
                    std::string ora;
                    std::string data;
                    std::string motivo("auto-ban 4 cheats.");
                    getDateAndTime(data,ora);
                    //non prendo la guid attuale, ma quella precedente (che spesso è quella vera e propria dell'utente prima che attivi i cheat)
                    database->ban(correggi(nick),ip,data,ora,correggi((*m_dati)[m_dati->serverNumber][i]->GUID),correggi(motivo),std::string());
                    block->kick(numero);
                }
                else
                {
                    #ifdef ITA
                    std::string phrase("^0BanBot warning: ^1il giocatore numero ");
                    phrase.append(numero);
                    phrase.append(", ");
                    phrase.append(nick);
                    phrase.append("^1 sta usando cheats (la guid e' cambiata).");
                    #else
                    std::string phrase("^0BanBot warning: ^1player number ");
                    phrase.append(numero);
                    phrase.append(", ");
                    phrase.append(nick);
                    phrase.append("^1 is using cheats (guid changed).");
                    #endif
                    tellToAdmins(phrase);
                }
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

    //if guid is enmpty, it's a problem...
    if (guid.empty() && (*m_dati)[m_dati->serverNumber].strict() >= LEVEL2)
    {
        //empty guid: cheat, or client touched (like qkey file deleted).
        if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL5 )
        {
            kicked=true;
            #ifdef ITA
                std::cout<<"  [!] ban automatico per GUID non valida (vuota).\n";
                *(m_dati->log)<<"  [!] ban automatico per GUID non valida (vuota).\n";
            #else
                std::cout<<"  [!] automated ban for non-valid GUID (empty).\n";
                *(m_dati->log)<<"  [!] automated ban for non-valid GUID (empty).\n";
            #endif

            #ifdef ITA
                std::string phrase("^0BanBot: ^1auto-ban al player numero ");
            #else
                std::string phrase("^0BanBot: ^1auto-ban player number ");
            #endif
            phrase.append(numero);
            phrase.append(", ");
            phrase.append(nick);
            #ifdef ITA
                phrase.append("^1 per QKey corrotta.");
            #else
                phrase.append("^1 for corrupted QKey.");
            #endif

            block->say(phrase);
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
            //Low strict level: i'll advice.
            #ifdef ITA
                std::string phrase("^0BanBot ^1attenzione: il player numero ");
            #else
                std::string phrase("^0BanBot ^1warning: player number ");
            #endif
            phrase.append(numero);
            phrase.append(", ");
            phrase.append(nick);
            #ifdef ITA
                phrase.append("^1 ha la QKey corrotta:\n^1probabilmente sta usando dei cheats.");
            #else
                phrase.append("^1 has a corrupted QKey:\n^1probably he's using cheats.");
            #endif
            tellToAdmins(phrase);
        }
    }

    //if all is ok, go with checks.
    if (!kicked)
    {
        //some checks: if he's banned, and others anticheat checks.
        if( !( guidIsBanned( guid, nick, numero, ip) || nickIsBanned( nick, numero, ip, guid ) || ipIsBanned( ip, numero, nick, guid )) )
        {
            //ok, non è stato bannato (per il momento). Controllo se ha un GUID valido.
            if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL2 && !guid.empty() && !isA(line, _R_GUID) )
            {
                //wrong guid.
                if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL4 )
                {
                    #ifdef ITA
                        std::cout<<"  [!] ban automatico per GUID illegale\n";
                        *(m_dati->log)<<"  [!] ban automatico per GUID illegale\n";
                        std::string phrase("^0BanBot: ^1kick automatico al giocatore ");
                    #else
                        std::cout<<"  [!] automated ban for illegal GUID.\n";
                        *(m_dati->log)<<"  [!] automated ban for illegal GUID.\n";
                        std::string phrase("^0BanBot: ^1kicking player number ");
                    #endif

                    phrase.append(numero);
                    phrase.append(", ");
                    phrase.append(nick);
                    #ifdef ITA
                        phrase.append("^1 per cheats.");
                    #else
                        phrase.append("^1 for cheats.");
                    #endif
                    block->say(phrase);
                    std::string ora;
                    std::string data;
                    std::string motivo("auto-ban 4 cheats.");
                    getDateAndTime(data,ora);
                    database->ban(correggi(nick),ip,data,ora,correggi(guid),correggi(motivo),std::string());
                    block->kick(numero);
                }
                else
                {
                    #ifdef ITA
                    std::cout<<"  [!] warning per GUID illegale\n";
                    *(m_dati->log)<<"  [!] warning per GUID illegale\n";
                    std::string phrase("^0BanBot warning: ^1il giocatore ");
                    #else
                    std::cout<<"  [!] warning for illegal GUID.\n";
                    *(m_dati->log)<<"  [!] warning for illegal GUID.\n";
                    std::string phrase("^0BanBot warning: ^1player number ");
                    #endif

                    phrase.append(numero);
                    phrase.append(", ");
                    phrase.append(nick);
                    #ifdef ITA
                    phrase.append("^1 ha dei cheat attivi.");
                    #else
                    phrase.append("^1 has cheats activated.");
                    #endif
                    tellToAdmins(phrase);
                }
            }
            else
            {
                //advanced checks:
                //TODO implement and activate
                if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL6 && !isA( line,_R_COMPLETE_CLIENT_USER_INFO ) )
                {
                    if ( (*m_dati)[m_dati->serverNumber].strict() >= LEVEL5 )
                    {
                        //automated action
                        #ifdef ITA
                            std::cout<<"  [!] kick automatico per client non pulito (controlli avanzati non superati).\n";
                            *(m_dati->log)<<"  [!] kick automatico per client non pulito (controlli avanzati non superati).\n";
                            std::string phrase("^0BanBot: ^1auto-kick al giocatore ");
                        #else
                            std::cout<<"  [!] automated kick for illegal client ( advanced checks not passed ).\n";
                            *(m_dati->log)<<"  [!] automated kick for illegal client ( advanced checks not passed ).\n";
                            std::string phrase("^0BanBot: ^1auto-kicking player number ");
                        #endif
                        phrase.append(numero);
                        phrase.append(", ");
                        phrase.append(nick);
                        #ifdef ITA
                            phrase.append("^1 per client non pulito.");
                        #else
                            phrase.append("^1 for illegal client.");
                        #endif
                        block->say(phrase);
                        block->kick(numero);
                    }
                    else
                    {
                        //only warning
                        #ifdef ITA
                        std::string phrase("^0BanBot ^1attenzione: il giocatore numero ");
                        #else
                        std::string phrase("^0BanBot ^1warning: player number ");
                        #endif
                        phrase.append(numero);
                        phrase.append(", ");
                        phrase.append(nick);
                        #ifdef ITA
                            phrase.append("^1 ha un client non pulito.");
                        #else
                            phrase.append("^1 has an illegal client.");
                        #endif
                        tellToAdmins(phrase);
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
    else delete block;
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
    //i check if i know the player and his permissions, if the player hasn't ban power, nothing to do.
    std::string numeroAdmin;
    int adminLevel = isAdminSay(line,numeroAdmin);
    if ( adminLevel <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::BAN))
    {
        InstructionsBlock * block = new InstructionsBlock();
        #ifdef ITA
            std::cout<<"  [OK] è un admin. Applico il ban.\n";
            *(m_dati->log)<<"  [OK] è un admin. Applico il ban.\n";
        #else
            std::cout<<"  [OK] Is an admin. Applying ban.\n";
            *(m_dati->log)<<"  [OK] Is an admin. Applying ban.\n";
        #endif
        //ok, do it.
        int i=0;
        //take the number or the nick of the player to ban
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
        //if i have the guid, ban it. I'll check if he is already banned too.
        if (i>=0 && !database->checkBanGuid((*m_dati)[m_dati->serverNumber][i]->GUID))
        {
            if ( adminLevel < database->checkAuthGuid((*m_dati)[m_dati->serverNumber][i]->GUID) )
            {
                #ifdef ITA
                    std::cout<<"  [+]banno "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" con guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    *(m_dati->log)<<"  [+]banno "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" con guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    std::string phrase("^0BanBot: ^1banno il giocatore ");
                #else
                    std::cout<<"  [+]banning "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" with guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    *(m_dati->log)<<"  [+]banning "<<(*m_dati)[m_dati->serverNumber][i]->nick<<" with guid "<<(*m_dati)[m_dati->serverNumber][i]->GUID<<"\n";
                    std::string phrase("^0BanBot: ^1banning player number ");
                #endif
                phrase.append((*m_dati)[m_dati->serverNumber][i]->number);
                phrase.append(", ");
                phrase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                if (!motivo.empty())
                {
                    #ifdef ITA
                        phrase.append("^1 per ");
                    #else
                        phrase.append("^1 for ");
                    #endif
                    phrase.append(motivo);
                }
                phrase.append(".");
                block->say(phrase);
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
                    std::cout<<"  [!]fail: il giocatore è un admin.\n";
                    *(m_dati->log)<<"  [!]fail: il giocatore è un admin.\n";
                    block->tell("^1Banning error: è un admin come te.",numeroAdmin);
                #else
                    std::cout<<"  [!]fail: player is an admin\n";
                    *(m_dati->log)<<"  [!]fail: player is an admin\n";
                    block->tell("^1Banning error: (s)he's an admin like you.",numeroAdmin);
                #endif
            }
        }
        else
        {
            #ifdef ITA
                std::cout<<"  [!]fail: giocatore sconosciuto o già bannato\n";
                *(m_dati->log)<<"  [!]fail: giocatore sconosciuto o già bannato\n";
                block->tell("^1Banning error: numero del giocatore sbagliato o nick non univoco.",numeroAdmin);
            #else
                std::cout<<"  [!]fail: unknown user or already banned\n";
                *(m_dati->log)<<"  [!]fail: unknown user or already banned\n";
                block->tell("^1Banning error: number of the player wrong or nick non unique.",numeroAdmin);
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::UNBAN))
    {
        InstructionsBlock * block = new InstructionsBlock();    // prepare instructionBlock
        //take the argument
        std::string temp(line);
        int pos=temp.find("!unban");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+6);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string player=temp.substr(pos,end-pos);

        if (!isA(line,_R_UNBAN_ID))
        {
            //search the precise nick on the database
            std::string phrase;
            std::vector<Db::idNickMotiveAuthorStruct> risultatoEsatto=database->findPreciseIdMotiveAuthorViaNickBanned(player);
            if (risultatoEsatto.size() == 1)  //if i found an exact match, i'll unban it
            {
                #ifdef ITA
                if (database->deleteBanned(risultatoEsatto[0].id))
                {
                    phrase.append("^0BanBot: ^1");
                    phrase.append(risultatoEsatto[0].nick);
                    phrase.append("^1 sbannato con successo.");
                }
                else phrase.append("^0BanBot: ^1è stato riscontrato un errore, utente non sbannato.");
                #else
                if (database->deleteBanned(risultatoEsatto[0].id))
                {
                    phrase.append("^0BanBot: ^1");
                    phrase.append(risultatoEsatto[0].nick);
                    phrase.append("^1 successfully unbanned.");
                }
                else phrase.append("^0BanBot: ^1error: player not unbanned.");
                #endif
            }
            else if ( risultatoEsatto.size() == 0 )
            {
                //search the approx nick on db
                std::vector<Db::idNickMotiveAuthorStruct> risultatoApprossimativo=database->findAproxIdMotiveAuthorViaNickBanned(player);
                if (risultatoApprossimativo.size() == 1)  //else, if i found an aprox match, i'll unban it
                {
                    #ifdef ITA
                    if (database->deleteBanned(risultatoApprossimativo[0].id))
                    {
                        phrase.append("^0BanBot: ^1");
                        phrase.append(risultatoApprossimativo[0].nick);
                        phrase.append("^1 sbannato con successo.");
                    }
                    else phrase.append("^0BanBot: ^1è stato riscontrato un errore, utente non sbannato.");
                    #else
                    if (database->deleteBanned(risultatoApprossimativo[0].id))
                    {
                        phrase.append("^0BanBot: ^1");
                        phrase.append(risultatoApprossimativo[0].nick);
                        phrase.append("^1 successfully unbanned.");
                    }
                    else phrase.append("^0BanBot: ^1error: player not unbanned.");
                    #endif
                }
                else
                {
                    if ( risultatoApprossimativo.size() > 1 )
                    {
                        #ifdef ITA
                        phrase.append("^0BanBot: ^1troppi risultati: specifica meglio il nick o usa !find");
                        #else
                        phrase.append("^0BanBot: ^1too results: use a complete nick or !find");
                        #endif
                    }
                    else
                    {
                        #ifdef ITA
                        phrase.append("^0BanBot: ^1nessun giocatore trovato da sbannare.");
                        #else
                        phrase.append("^0BanBot: ^1player not found");
                        #endif
                    }
                }
            }
            else
            {
                //too players
                #ifdef ITA
                phrase.append("^0BanBot: ^1troppi risultati: specifica meglio il nick o usa !find");
                #else
                phrase.append("^0BanBot: ^1too results: use a complete nick or !find");
                #endif
            }
            block->tell(phrase,numeroAdmin);
        }
        else
        {
            //i have the number, i'll delete it from database
            std::string phrase;
            #ifdef ITA
                if (database->deleteBanned(player))
                    phrase.append("^0BanBot: ^1utente sbannato con successo.");
                else phrase.append("^0BanBot: ^1è stato riscontrato un errore, utente non sbannato.");
            #else
                if (database->deleteBanned(player))
                    phrase.append("^0BanBot: ^1player successfully unbanned.");
                else phrase.append("^0BanBot: ^1error: player not unbanned.");
            #endif
            block->tell(phrase,numeroAdmin);
        }
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
    if (isAdminSay(line,numero) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::UNBAN))
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
            std::string phrase("^2Risultati esatti: \n^1");
        #else
            std::cout<<"search: "<<risultato.size()<<" "<<risultatoApprossimativo.size()<<"\n";
            std::string phrase("^2Exact results: \n^1");
        #endif
        if (risultato.size()>5)
        {
            #ifdef ITA
                phrase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                phrase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            phrase.append(risultato[0].id);
            phrase.append("^2:^1 ");
            phrase.append(risultato[0].nick);
            phrase.append("^1 ");
            phrase.append(risultato[0].motive);
            phrase.append(" ^2by^1 ");
            phrase.append(risultato[0].author);
            phrase.append(" ^2?");
        }
        else
        {
            if (risultato.empty()) phrase.append("none.");
            else for (unsigned int i=0; i<risultato.size();i++)
            {
                phrase.append(risultato[i].id);
                phrase.append("^2:^1 ");
                phrase.append(risultato[i].nick);
                phrase.append("^1 ");
                phrase.append(risultato[i].motive);
                phrase.append(" ^2by^1 ");
                phrase.append(risultato[i].author);
                if(i<risultato.size()-1) phrase.append(",\n^1");
                else phrase.append(".");
            }
        }
        block->tell(phrase,numero);

        phrase.clear();
        #ifdef ITA
            phrase.append("^2Risultati ricerca: \n^1");
        #else
            phrase.append("^2Search results: \n^1");
        #endif
        if (risultatoApprossimativo.size()>15)
        {
            #ifdef ITA
                phrase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                phrase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            phrase.append(risultatoApprossimativo[0].id);
            phrase.append("^2:^1 ");
            phrase.append(risultatoApprossimativo[0].nick);
            phrase.append("^1 ");
            phrase.append(risultatoApprossimativo[0].motive);
            phrase.append(" ^2by^1 ");
            phrase.append(risultatoApprossimativo[0].author);
            #ifdef ITA
                phrase.append("\n^2, o forse \n ^1");
            #else
                phrase.append("\n^2, or \n ^1");
            #endif
            phrase.append(risultatoApprossimativo[1].id);
            phrase.append("^2:^1 ");
            phrase.append(risultatoApprossimativo[1].nick);
            phrase.append("^1 ");
            phrase.append(risultatoApprossimativo[1].motive);
            phrase.append(" ^2by^1 ");
            phrase.append(risultatoApprossimativo[1].author);
            phrase.append(" ^2?");
        }
        else
        {
            if (risultatoApprossimativo.empty()) phrase.append("none.");
            else for (unsigned int i=0; i<risultatoApprossimativo.size();i++)
            {
                phrase.append(risultatoApprossimativo[i].id);
                phrase.append("^2:^1 ");
                phrase.append(risultatoApprossimativo[i].nick);
                phrase.append("^1 ");
                phrase.append(risultatoApprossimativo[i].motive);
                phrase.append(" ^2by^1 ");
                phrase.append(risultatoApprossimativo[i].author);
                if(i<risultatoApprossimativo.size()-1) phrase.append(",\n^1");
                else phrase.append(".");
            }
        }
        block->tell(phrase,numero);
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
    if (isAdminSay(line,numero) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::DEOP))
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
            #ifdef DEBUG_MODE
            std::cout<<"ricerca: "<<risultato.size()<<" "<<risultatoApprossimativo.size()<<"\n";
            #endif
            std::string phrase("^2Risultati esatti: \n^1");
        #else
            #ifdef DEBUG_MODE
            std::cout<<"search: "<<risultato.size()<<" "<<risultatoApprossimativo.size()<<"\n";
            #endif
            std::string phrase("^2Exact results: \n^1");
        #endif
        if (risultato.size()>5)
        {
            #ifdef ITA
                phrase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                phrase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            phrase.append(risultato[0].id);
            phrase.append("^2:^1 ");
            phrase.append(risultato[0].nick);
            phrase.append("^2 l^1 ");
            phrase.append(risultato[0].level);
            phrase.append(" ^2?");
        }
        else
        {
            if (risultato.empty()) phrase.append("none.");
            else for (unsigned int i=0; i<risultato.size();i++)
            {
                phrase.append(risultato[i].id);
                phrase.append("^2:^1 ");
                phrase.append(risultato[i].nick);
                phrase.append("^2 l^1 ");
                phrase.append(risultato[i].level);
                if(i<risultato.size()-1) phrase.append(",\n^1");
                else phrase.append(".");
            }
        }
        block->tell( phrase, numero );

        phrase.clear();
        #ifdef ITA
            phrase.append("^2Risultati ricerca: \n^1");
        #else
            phrase.append("^2Search results: \n^1");
        #endif
        if (risultatoApprossimativo.size()>15)
        {
            #ifdef ITA
                phrase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi\n^1");
            #else
                phrase.append("too many results, try to improve research. Were you looking for\n^1");
            #endif
            phrase.append(risultatoApprossimativo[0].id);
            phrase.append("^2:^1 ");
            phrase.append(risultatoApprossimativo[0].nick);
            phrase.append("^2 l^1");
            phrase.append(risultatoApprossimativo[0].level);
            phrase.append("\n^2, o forse \n^1");
            phrase.append(risultatoApprossimativo[1].id);
            phrase.append("^2:^1 ");
            phrase.append(risultatoApprossimativo[1].nick);
            phrase.append("^2 l ^1");
            phrase.append(risultatoApprossimativo[1].level);
            phrase.append(" ^2?");
        }
        else
        {
            if (risultatoApprossimativo.empty()) phrase.append("none.");
            else for (unsigned int i=0; i<risultatoApprossimativo.size();i++)
            {
                phrase.append(risultatoApprossimativo[i].id);
                phrase.append("^2:^1 ");
                phrase.append(risultatoApprossimativo[i].nick);
                phrase.append("^2 l^1 ");
                phrase.append(risultatoApprossimativo[i].level);
                if(i<risultatoApprossimativo.size()-1) phrase.append(",\n^1");
                else phrase.append(".");
            }
        }
        block->tell(phrase,numero);
        m_scheduler->addInstructionBlock( block, Server::LOW );
    }
}

void Analyzer::op(char* line)
{
    std::cout<<"[!] Op";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Op";

    std::string numeroAdmin;
    int level = isAdminSay(line,numeroAdmin);
    if (level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::OP))
    {
        InstructionsBlock * block = new InstructionsBlock();
        int i=0;
        //prendo il numero o nome del player da aggiungere tra gli admin
        std::string temp(line);
        int pos=temp.find("!op");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+3);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string player=temp.substr(pos,end-pos);
        pos=temp.find_first_not_of(" \t\n\r\f\v",end);
        end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string newOpLevel;
        if (end != pos) newOpLevel = temp.substr(pos, end-pos); 
        
        if (isA(newOpLevel.c_str(),_R_NUMBER))
        {
            int opLevel = level;
            opLevel=atoi(newOpLevel.c_str());
            //i don't permit an highter level of the admin
            if ( opLevel < level )
                opLevel = level;
            newOpLevel = handyFunctions::intToString(opLevel);
        }
        else
        {
            newOpLevel = handyFunctions::intToString(level);
        }
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
            if( database->addOp((*m_dati)[m_dati->serverNumber][i]->nick,(*m_dati)[m_dati->serverNumber][i]->GUID, newOpLevel) )
            {
                std::string phrase ( "^0BanBot: ^1" );
                phrase.append( (*m_dati)[m_dati->serverNumber][i]->nick );
                #ifdef ITA
                    phrase.append("^1 aggiunto con successo agli admin, livello ");
                #else
                    phrase.append("^1 successfully added to admin list, level ");
                #endif
                phrase.append ( newOpLevel );
                block->tell( phrase, numeroAdmin );
            }
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
    //check user's powa
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::DEOP))
    {
        InstructionsBlock * block = new InstructionsBlock();
        //take player's data
        std::string temp(line);
        int pos=temp.find("!deop");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+5);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string player=temp.substr(pos,end-pos);
        
        
        if (!isA(line,_R_DEOP_ID))
        {
            //search the precise nick on the database
            std::string phrase;
            std::vector<Db::idNickStruct> risultatoEsatto=database->findPreciseIdNickViaNickOp(player);
            if (risultatoEsatto.size() == 1)  //if i found an exact match, i'll unban it
            {
                #ifdef ITA
                if (database->deleteOp(risultatoEsatto[0].id))
                {
                    phrase.append("^0BanBot: ^2");
                    phrase.append(risultatoEsatto[0].nick);
                    phrase.append(" ^1 e' stato tolto dalla lista admin.");
                }
                else phrase.append("^0BanBot: ^1è stato riscontrato un errore, admin non eliminato.");
                #else
                if (database->deleteOp(risultatoEsatto[0].id))
                {
                    phrase.append("^0BanBot: ^2");
                    phrase.append(risultatoEsatto[0].nick);
                    phrase.append(" ^1successfully removed from admin list.");
                }
                else phrase.append("^0BanBot: ^1error: admin not deleted.");
                #endif
            }
            else if ( risultatoEsatto.size() == 0 )
            {
                //search the approx nick on db
                std::vector<Db::idNickStruct> risultatoApprossimativo=database->findAproxIdNickViaNickOp(player);
                if (risultatoApprossimativo.size() == 1)  //else, if i found an aprox match, i'll unban it
                {
                    #ifdef ITA
                    if (database->deleteOp(risultatoApprossimativo[0].id))
                    {
                        phrase.append("^0BanBot: ^2");
                        phrase.append(risultatoApprossimativo[0].nick);
                        phrase.append(" ^1 e' stato tolto dalla lista admin.");
                    }
                    else phrase.append("^0BanBot: ^1è stato riscontrato un errore, admin non eliminato.");
                    #else
                    if (database->deleteOp(risultatoApprossimativo[0].id))
                    {
                        phrase.append("^0BanBot: ^2");
                        phrase.append(risultatoApprossimativo[0].nick);
                        phrase.append(" ^1successfully removed from admin list.");
                    }
                    else phrase.append("^0BanBot: ^1error: admin not deleted.");
                    #endif
                }
                else
                {
                    if ( risultatoApprossimativo.size() > 1 )
                    {
                        #ifdef ITA
                        phrase.append("^0BanBot: ^1troppi risultati: specifica meglio il nick o usa !findop");
                        #else
                        phrase.append("^0BanBot: ^1too results: use a complete nick or !findop");
                        #endif
                    }
                    else
                    {
                        #ifdef ITA
                        phrase.append("^0BanBot: ^1nessun admin trovato da eliminare.");
                        #else
                        phrase.append("^0BanBot: ^1player not found.");
                        #endif
                    }
                }
            }
            else
            {
                //too players
                #ifdef ITA
                phrase.append("^0BanBot: ^1troppi risultati: usa !findop");
                #else
                phrase.append("^0BanBot: ^1too results: use !findop");
                #endif
            }
            block->tell(phrase,numeroAdmin);
        }
        else
        {
            //i have the number, i'll delete it from database
            std::string phrase;
            if (database->deleteOp(player))
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
            block->tell(phrase,numeroAdmin);
        }
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::KICK))
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
                std::string phrase("^0BanBot: ^1butto fuori il player numero ");
            #else
                std::string phrase("^0BanBot: ^1kicking player number ");
            #endif
            phrase.append(player);
            phrase.append("...");
            block->say(phrase);
            block->kick(player);
        }
        else
        {
            int i=translatePlayer(player);
            if (i<0)
            {
                #ifdef ITA
                    block->tell("^1Errore: nick non trovato o non univoco.",numeroAdmin);
                    *(m_dati->log)<<"\n[ERROR] player non trovato.\n";
                #else
                    block->tell("^1Error: nick not unique or not found.",numeroAdmin);
                    *(m_dati->log)<<"\n[ERROR] player not found.\n";
                #endif
            }
            else
            {
                #ifdef ITA
                    std::string phrase("^0BanBot: ^1butto fuori ");
                    *(m_dati->log)<<"\n[OK] butto fuori il giocatore.\n";
                #else
                    std::string phrase("^0BanBot: ^1kicking ");
                    *(m_dati->log)<<"\n[OK] kicking player.\n";
                #endif
                phrase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                phrase.append("^1...");
                block->say(phrase);
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::MUTE))
    {
        InstructionsBlock * block = new InstructionsBlock();
        if ( isA(line,_R_MUTE_ALL) )
        {
            #ifdef ITA
            std::string phrase("^0BanBot: ^1muto/smuto tutti.");
            #else
            std::string phrase("^0BanBot: ^1muting/unmuting all players.");
            #endif
            block->say(phrase);
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
                    std::string phrase("^0BanBot: ^1muto/smuto il giocatore numero ");
                #else
                    std::string phrase("^0BanBot: ^1muting/unmuting player number ");
                #endif
                phrase.append(player);
                phrase.append("...");
                block->say(phrase);
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
                        std::string phrase("^0BanBot: ^1muto/smuto ");
                    #else
                        std::string phrase("^0BanBot: ^1muting/unmuting ");
                    #endif
                    phrase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                    phrase.append("^1...");
                    block->say(phrase);
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
    int level = isAdminSay(line,numero);
    if (level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::HELP))
    {
        InstructionsBlock * block = new InstructionsBlock();
        //creation of the phrase to show
        std::string phrase( COMMANDLIST );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::KICK) )
            phrase.append( H_KICK );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::BAN) )
            phrase.append( H_BAN );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::UNBAN) )
        {
            phrase.append( H_FIND );
            phrase.append( H_UNBAN );
        }
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::MUTE) )
            phrase.append( H_MUTE );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::OP) )
            phrase.append( H_OP );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::DEOP) )
        {
            phrase.append( H_FINDOP );
            phrase.append( H_DEOP );
        }
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::STRICT) )
            phrase.append( H_STRICT );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::NUKE) )
            phrase.append( H_NUKE );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::SLAP) )
            phrase.append( H_SLAP );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::VETO) )
            phrase.append( H_VETO );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::FORCE) )
            phrase.append( H_FORCE );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::MAP) )
            phrase.append( H_MAP );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::NEXTMAP) )
            phrase.append( H_NEXTMAP );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::ADMINS) )
            phrase.append( H_ADMINS );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::PASS) )
            phrase.append( H_PASS );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::CONFIG) )
            phrase.append( H_CONFIG );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::STATUS) )
            phrase.append( H_STATUS );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::WARNINGS) )
            phrase.append( H_WARNINGS );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::BANTIMEWARN) )
            phrase.append( H_BANTIMEWARN );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::TEAMS) )
            phrase.append( H_TEAMS );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::TEAMBALANCE) )
            phrase.append( H_BALANCE );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::GRAVITY) )
            phrase.append( H_GRAVITY );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::CHANGELEVEL) )
            phrase.append( H_CHANGELEVEL );
        if ( level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::BIGTEXT) )
            phrase.append( H_BIGTEXT );

        phrase.append( H_LEVEL );
        phrase.append( handyFunctions::intToString(level) );
        block->tell(phrase,numero);
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::STRICT))
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
                std::string phrase("^0BanBot: ^1Strict ora è al livello ");
            #else
                std::string phrase("^0BanBot: ^1Strict mode now is on level ");
            #endif
            phrase.append(variable);
            block->tell(phrase,numeroAdmin);
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::VETO))
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::SLAP))
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
            pos=temp.find_first_of("23456789",end);
            mul_string=temp.at(pos);
            multiplier=atoi(mul_string.c_str());
        }
        if (isA(line,_R_SLAP_NUMBER))
        {
            #ifdef ITA
                std::string phrase("^0BanBot: ^1slappo il giocatore numero ");
            #else
                std::string phrase("^0BanBot: ^1slapping player number ");
            #endif
            phrase.append(player);
            phrase.append(" ");
            phrase.append(mul_string);
            #ifdef ITA
                phrase.append(" volta(e)...");
            #else
                phrase.append(" time(s)...");
            #endif
            block->say(phrase);
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
                    std::string phrase("^0BanBot: ^1slappo ");
                #else
                    std::string phrase("^0BanBot: ^1slapping ");
                #endif
                phrase.append((*m_dati)[m_dati->serverNumber][number]->nick);
                phrase.append("^1 ");
                phrase.append(mul_string);
                #ifdef ITA
                    phrase.append(" volta(e)...");
                #else
                    phrase.append(" time(s)...");
                #endif
                block->say(phrase);
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::NUKE))
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
                std::string phrase("^0BanBot: ^1nuke al giocatore numero ");
            #else
                std::string phrase("^0BanBot: ^1nuking player number ");
            #endif
            phrase.append(player);
            phrase.append(".");
            block->say(phrase);
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
                    std::string phrase("^0BanBot: ^1nuke a ");
                #else
                    std::string phrase("^0BanBot: ^1nuking ");
                #endif
                phrase.append((*m_dati)[m_dati->serverNumber][number]->nick);
                phrase.append("^1.");
                block->say(phrase);
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
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::STATUS))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string phrase( "^0BanBot ^1status: version " );
        phrase.append( _VERSION );
        phrase.append( ", coded by [2s2h]n3m3s1s & [2s2h]Zamy.\n^1Strict level: ^2" );
        switch ((*m_dati)[m_dati->serverNumber].strict())
        {
            case 0:
                phrase.append("OFF");
                break;
            case 1:
                phrase.append("1");
                break;
            case 2:
                phrase.append("2");
                break;
            case 3:
                phrase.append("3");
                break;
            case 4:
                phrase.append("4");
                break;
            case 5:
                phrase.append("5");
                break;
            default:
                phrase.append("6");
                break;
        }
        #ifdef ITA
            phrase.append("\n^1Numero degli admin: ^2");
            phrase.append(database->ops());
            phrase.append("\n^1Giocatori bannati: ^2");
            phrase.append(database->banned());
            phrase.append("\n^1Giocatori bannati automaticamente: ^2");
            phrase.append(database->autoBanned());
            phrase.append("\n^1Messaggi warning: ^2");
            if ((*m_dati)[(*m_dati).serverNumber].warnings() == Server::DISABLED)
                phrase.append("disattivati");
            else if ((*m_dati)[(*m_dati).serverNumber].warnings() == Server::PUBLIC)
                phrase.append("pubblici");
            else phrase.append("privati");
            phrase.append("\n^1Warning ban scaduti: ^2");
            if ((*m_dati)[(*m_dati).serverNumber].banWarnings())
                phrase.append("on");
            else
                phrase.append("off");
        #else
            phrase.append("\n^1Number of admins: ^2");
            phrase.append(database->ops());
            phrase.append("\n^1Players currently banned: ^2");
            phrase.append(database->banned());
            phrase.append("\n^1Players banned automatically: ^2");
            phrase.append(database->autoBanned());
            phrase.append("\n^1Warning messages: ^2");
            if ((*m_dati)[(*m_dati).serverNumber].warnings() == Server::DISABLED)
                phrase.append("disabled");
            else if ((*m_dati)[(*m_dati).serverNumber].warnings() == Server::PUBLIC)
                phrase.append("public");
            else phrase.append("private");
            phrase.append("\n^1Old bans warnings: ^2");
            if ((*m_dati)[(*m_dati).serverNumber].banWarnings())
                phrase.append("on");
            else
                phrase.append("off");
        #endif
        block->say(phrase);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::force(char* line)
{
    std::cout<<"[!] Force";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Force";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::FORCE))
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

        std::string phrase;
        if (isA(line,_R_FORCE_NUMBER))
        {
            #ifdef ITA
                phrase.append("^0BanBot: ^1sposto il giocatore numero ");
                phrase.append(player);
                phrase.append(" in ");
            #else
                phrase.append("^0BanBot: ^1forcing player number ");
                phrase.append(player);
                phrase.append(" to ");
            #endif
            phrase.append(action);
            phrase.append(".");
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
                    phrase.append("^0BanBot: ^1sposto ");
                    phrase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                    phrase.append("^1 in ");
                #else
                    phrase.append("^0BanBot: ^1forcing ");
                    phrase.append((*m_dati)[m_dati->serverNumber][i]->nick);
                    phrase.append("^1 to ");
                #endif
                phrase.append(action);
                phrase.append(".");
                player=(*m_dati)[m_dati->serverNumber][i]->number;
            }
        }
        block->say(phrase);
        block->force(player,action);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::iamgod(char* line)
{
    std::cout<<"[!] iamgod";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] iamgod";
    //this command have to work only when there aren't admins, it exists only to simplify the installation
    if (database->isOpTableEmpty())   //if the oplist table is empty, i should execute this command.
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp = line;
        int pos = temp.find( "say:" );
        pos = temp.find_first_not_of( " ", pos+4 );
        int end = temp.find_first_of( " ", pos );
        std::string numero = temp.substr( pos, end-pos );
        //i take player infos, and then add him in op list with level 0 (hightest admin level).
        int i = findPlayer( numero );
        if( i>=0 && database->addOp( correggi((*m_dati)[m_dati->serverNumber][i]->nick), correggi((*m_dati)[m_dati->serverNumber][i]->GUID), "0") )
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
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::MAP))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!map");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+4);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string map=temp.substr(pos,end-pos);
        pos=temp.find_first_not_of(" \t\n\r\f\v",end);
        end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string map2;
        if (end != pos) map2=temp.substr(pos, end-pos);
        
        int listIndex=translateMap(map, map2);
        if (listIndex>=0)
        {
            block->map(m_dati->currentServer()->map(listIndex));
        }
        else
        {
            #ifdef ITA
            block->tell("^0BanBot: ^1Errore: mappa richiesta non trovata.",numeroAdmin);
            #else
            block->tell("^0BanBot: ^1Error: can't find the requested map.",numeroAdmin);
            #endif
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::nextmap(char* line)
{
    std::cout<<"[!] NextMap";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] NextMap";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::NEXTMAP))
    {
        InstructionsBlock * block = new InstructionsBlock();
        //prendo il numero o nome del player da aggiungere tra gli admin
        std::string temp(line);
        int pos=temp.find("!nextmap");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+8);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string map=temp.substr(pos,end-pos);
        pos=temp.find_first_not_of(" \t\n\r\f\v",end);
        end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string map2;
        if (end != pos) map2=temp.substr(pos, end-pos);
        
        int listIndex=translateMap(map, map2);
        if (listIndex>=0)
        {
            block->nextmap(m_dati->currentServer()->map(listIndex));
        }
        else
        {
            #ifdef ITA
            block->tell("^0BanBot: ^1Errore: mappa richiesta non trovata.",numeroAdmin);
            #else
            block->tell("^0BanBot: ^1Error: can't find the requested map.",numeroAdmin);
            #endif
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::admins(char* line)
{
    std::cout<<"[!] Admins";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Admins";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::ADMINS))
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
            mex.append( " l " );
            mex.append( handyFunctions::intToString(database->checkAuthGuid((*m_dati)[m_dati->serverNumber][t[i]]->GUID)) );
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::PASS))
    {
        InstructionsBlock* block = new InstructionsBlock();
        std::string password("");
        std::string phrase("");
        if (isA(line, _R_PASS)){
            
            std::string temp = line;
            int pos = temp.find( "!pass" );
            pos = temp.find_first_not_of( " ", pos+5 );
            password = temp.substr( pos );

            #ifdef ITA
                phrase.append("^0BanBot:^1 password del server cambiata in ^2");
            #else
                phrase.append("^0BanBot:^1 server's password changed to ^2");
            #endif
            phrase.append( password );
        }
        else {
            #ifdef ITA
            phrase.append("^0BanBot:^1 il server ora e' pubblico.");
            #else
            phrase.append("^0BanBot:^1 the server now is public.");
            #endif
        }
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
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::CONFIG))
    {
        InstructionsBlock *block = new InstructionsBlock();
        std::string temp = line;
        int pos = temp.find( "!config" );
        pos = temp.find_first_not_of( " \t\n\r\f\v", pos+7 );
        int end = temp.find_first_of( " \t\n\r\f\v", pos );
        std::string conf1 = temp.substr( pos, end-pos );
        pos = temp.find_first_not_of( " \t\n\r\f\v", end );
        end = temp.find_first_of( " \t\n\r\f\v", pos );
        std::string conf2;
        if (end != pos) conf2=temp.substr(pos, end-pos);
        
        int listIndex = translateConfig(conf1,conf2);
        
        if (listIndex >= 0)
        {
            std::string file = m_dati->currentServer()->config(listIndex);
            #ifdef ITA
            std::string phrase ("^0BanBot:^1 carico il file di configurazione ^2");
            #else
            std::string phrase ("^0BanBot:^1 loading the configuration file ^2");
            #endif

            phrase.append( file );
            block->say(phrase);
            block->exec( file );
        }
        else
        {
            #ifdef ITA
            block->tell("^0BanBot:^1 Errore: file di configurazione non trovato.",numeroAdmin);
            #else
            block->tell("^0BanBot:^1 Error: config file not found.",numeroAdmin);
            #endif
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::warnings(char* line)
{
    std::cout<<"[!] Warnings";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Warnings";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::WARNINGS))
    {
        InstructionsBlock* block = new InstructionsBlock();
        std::string temp = line;
        int pos = temp.find( "!warnings" );
        pos = temp.find_first_not_of( " ", pos+9 );
        std::string type = temp.substr( pos );
        if (type.compare("off") == 0)
        {
            (*m_dati)[(*m_dati).serverNumber].setWarnings(Server::DISABLED);
            #ifdef ITA
            block->tell("^0BanBot:^1 i messaggi di warning adesso sono ^2disabilitati.",numeroAdmin);
            #else
            block->tell("^0BanBot:^1 warning messages now are ^2disabled.",numeroAdmin);
            #endif
        }
        else if (type.compare("public") == 0)
        {
            #ifdef ITA
            block->tell("^0BanBot:^1 i messaggi di warning adesso sono ^2pubblici.",numeroAdmin);
            #else
            block->tell("^0BanBot:^1 warning messages now are ^2public.",numeroAdmin);
            #endif
        }
        else
        {
            (*m_dati)[(*m_dati).serverNumber].setWarnings(Server::PRIVATE);
            #ifdef ITA
            block->tell("^0BanBot:^1 i messaggi di warning adesso sono ^2privati.",numeroAdmin);
            #else
            block->tell("^0BanBot:^1 warning messages now are ^2private.",numeroAdmin);
            #endif
        }
        m_scheduler->addInstructionBlock(block, Server::MEDIUM);
    }
}

void Analyzer::bantimewarn(char* line)
{
    std::cout<<"[!] BanTimeWarn";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] BanTimeWarn";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::BANTIMEWARN))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!bantimewarn");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+12);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string option=temp.substr(pos,end-pos);
        
        std::string phrase;
        #ifdef ITA
        phrase.append("^0BanBot: ^1Gli avvisi per i vecchi ban ora sono ^2"); 
        #else
        phrase.append("^0BanBot: ^1Old bans warnings now are ^2"); 
        #endif
        if (option.compare("off")==0)
        {
            (*m_dati)[(*m_dati).serverNumber].setBanWarnings(false);
            #ifdef ITA
            phrase.append("disattivati^1."); 
            #else
            phrase.append("disabled^1.");
            #endif
        }
        else
        {
            (*m_dati)[(*m_dati).serverNumber].setBanWarnings(true);
            #ifdef ITA
            phrase.append("attivati^1."); 
            #else
            phrase.append("enabled^1.");
            #endif
        }
        
        
        block->tell(phrase,numeroAdmin);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::restart(char* line)
{
    std::cout<<"[!] Restart";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Restart";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::RESTART))
    {
        InstructionsBlock *block = new InstructionsBlock();
        
        #ifdef ITA
        std::string phrase ("^0BanBot:^1 Partita riavviata: ^2 hf!");
        #else
        std::string phrase ("^0BanBot:^1 Match restarted: ^2 hf!");
        #endif
        
        block->restart();
        block->say(phrase);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::reload(char* line)
{
    std::cout<<"[!] Reload";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Reload";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::RESTART))
    {
        InstructionsBlock *block = new InstructionsBlock();
        
        #ifdef ITA
        std::string phrase ("^0BanBot:^2 reload ^1in corso.");
        #else
        std::string phrase ("^0BanBot: ^1doing a^2 reload.");
        #endif
        
        block->say(phrase);
        block->reload();
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::balance(char* line)
{
    std::cout<<"[!] Team balance";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Team balance";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::TEAMBALANCE))
    {
        InstructionsBlock *block = new InstructionsBlock();
        if ( m_dati->currentServer()->permitBalance() ){
            #ifdef ITA
            std::string phrase ("^0BanBot:^2 bilanciamento team ^1in corso.");
            #else
            std::string phrase ("^0BanBot: ^2 balancing teams.");
            #endif
            
            block->say(phrase);
            block->teamBalance( m_dati->currentServer()->getSpectNumbers() );
        }
        else {
            #ifdef ITA
            std::string phrase ("^0BanBot: ^1Bilanciamento gia' eseguito.");
            #else
            std::string phrase ("^0BanBot: ^1Team balance already done.");
            #endif
            block->tell(phrase,numeroAdmin);
        }
        m_scheduler->addInstructionBlock( block, Server::LOW );
    }
}

void Analyzer::gravity(char* line)
{
    std::cout<<"[!] Gravity";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Gravity";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::GRAVITY))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!grav");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+5);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string option=temp.substr(pos,end-pos);
        
        std::string phrase;
        #ifdef ITA
        phrase.append("^0BanBot: ^1La nuova gravita' e' ^2"); 
        #else
        phrase.append("^0BanBot: ^1The new gravity is ^2"); 
        #endif
        if (option.compare("off")==0)
        {
            (*m_dati)[(*m_dati).serverNumber].setBanWarnings(false);
            phrase.append("800^1."); 
            block->gravity("800");
        }
        else
        {
            (*m_dati)[(*m_dati).serverNumber].setBanWarnings(true);
            phrase.append(option);
            phrase.append("^1.");
            block->gravity(option);
        }
        
        block->say(phrase);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::changeLevel(char* line)
{
    std::cout<<"[!] Change admin level";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Change admin level";
    
    std::string numeroAdmin;
    int level = isAdminSay(line,numeroAdmin);
    if (level <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::CHANGELEVEL))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string id;
        std::string oldLevel;
        //take the id or the number of the admin that i have to modify.
        std::string temp(line);
        int pos=temp.find("!level");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+6);
        int end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string player=temp.substr(pos,end-pos);
        pos=temp.find_first_of("0123456789",end);
        end=temp.find_first_of(" \t\n\r\f\v",pos);
        std::string newOpLevel=temp.substr(pos, end-pos);
        
        //i don't permit an highter level of the admin
        if ( atoi(newOpLevel.c_str()) < level ){ 
            newOpLevel = handyFunctions::intToString(level);
        }
        
        if (isA(line,_R_CHANGELEVEL_NUMBER)){ //i already have the id if the admin's record
            id = player;
            oldLevel = database->opStruct(id).level;
        }
        else{ //try to get the admin's id
            std::vector< Db::idNickStruct > precise = database->findPreciseIdNickViaNickOp( player );
            if ( precise.size() == 0 )
            {
                std::vector< Db::idNickStruct > approx = database->findAproxIdNickViaNickOp( player );
                if ( approx.size() == 0 )
                    #ifdef ITA
                    block->tell("^1Errore: admin non trovato.",numeroAdmin);
                    #else
                    block->tell("^1Error: admin not found.",numeroAdmin);
                    #endif
                else if ( approx.size() == 1 ){
                    id = approx[0].id;
                    oldLevel = approx[0].level;
                }
                else {
                    std::string phrase("^0BanBot: ^2");
                    #ifdef ITA
                    phrase.append("forse cercavi\n^1");
                    #else
                    phrase.append("are you looking for\n^1");
                    #endif
                    phrase.append(approx[0].id);
                    phrase.append("^2:^1 ");
                    phrase.append(approx[0].nick);
                    phrase.append("^2 l^1 ");
                    phrase.append(approx[0].level);
                    phrase.append(" ^2");
                    #ifdef ITA
                    phrase.append("oppure ^1");
                    #else
                    phrase.append("or maybe ^1");
                    #endif
                    phrase.append(approx[1].id);
                    phrase.append("^2:^1 ");
                    phrase.append(approx[1].nick);
                    phrase.append("^2 l^1 ");
                    phrase.append(approx[1].level);
                    phrase.append(" ^2?");
                    block->tell(phrase,numeroAdmin);
                }
            }
            else if ( precise.size() == 1 ){
                id = precise[0].id;
                oldLevel = precise[0].level;
            }
            else {
                std::string phrase("^0BanBot: ^2");
                #ifdef ITA
                phrase.append("forse cercavi\n^1");
                #else
                phrase.append("are you looking for\n^1");
                #endif
                phrase.append(precise[0].id);
                phrase.append("^2:^1 ");
                phrase.append(precise[0].nick);
                phrase.append("^2 l^1 ");
                phrase.append(precise[0].level);
                phrase.append(" ^2");
                #ifdef ITA
                phrase.append("oppure ^1");
                #else
                phrase.append("or maybe ^1");
                #endif
                phrase.append(precise[1].id);
                phrase.append("^2:^1 ");
                phrase.append(precise[1].nick);
                phrase.append("^2 l^1 ");
                phrase.append(precise[1].level);
                phrase.append(" ^2?");
                block->tell(phrase,numeroAdmin);
            }
        }
        
        //ok, let's do it!
        if( !id.empty() && !oldLevel.empty() ){
            if ( atoi(oldLevel.c_str()) >= level )
            {
                std::string phrase ( "^0BanBot: ^1" );
                if ( database->modifyOp(id,"","",newOpLevel) ){
                    #ifdef ITA
                    phrase.append(" amministratore modificato con successo al l. ");
                    #else
                    phrase.append(" admin successfully modified at l. ");
                    #endif
                    phrase.append ( newOpLevel );
                }
                else
                    #ifdef ITA
                    phrase.append(" fail, admin non modificato.");
                    #else
                    phrase.append(" fail, admin not modified.");
                    #endif 
                block->tell( phrase, numeroAdmin );
            }
            else
                #ifdef ITA
                block->tell("^0BanBot: ^1permessi insufficienti per modificare questo admin.",numeroAdmin);
                #else
                block->tell("^0BanBot: ^1insufficient permissions to modify this admin.",numeroAdmin);
                #endif
        }
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::bigtext(char* line)
{
    std::cout<<"[!] Bigtext";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Bigtext";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::BIGTEXT))
    {
        InstructionsBlock * block = new InstructionsBlock();
        std::string temp(line);
        int pos=temp.find("!bigtext");
        pos=temp.find_first_not_of(" \t\n\r\f\v",pos+8);
        std::string option=temp.substr(pos);
        
        block->bigtext(option);
        m_scheduler->addInstructionBlock( block, Server::MEDIUM );
    }
}

void Analyzer::teams(char* line)
{
    std::cout<<"[!] Teams";
    (m_dati->log)->timestamp();
    *(m_dati->log)<<"\n[!] Teams";
    //i check the player and his permissions, if he isn't autorized to use this command, nothing to do.
    std::string numeroAdmin;
    if (isAdminSay(line,numeroAdmin) <= (*m_dati)[(*m_dati).serverNumber].commandPermission(Server::TEAMS))
    {
        InstructionsBlock *block = new InstructionsBlock();
        if ( m_dati->currentServer()->permitTeams() ){
            //counting player for each team
            int red = 0;
            int blue = 0;
            for( unsigned int i = 0; i < m_dati->currentServer()->size(); i++ ){
                if( (*m_dati->currentServer())[i]->team == Server::Player::RED ) red++; 
                else if( (*m_dati->currentServer())[i]->team == Server::Player::BLUE ) blue++;
            }
            #ifdef DEBUG_MODE
            std::cout<<"Red found: "<<red<<" Blue found: "<<blue<<" Total: "<<m_dati->currentServer()->size()<<"\n";
            #endif
            
            if (red != blue)
            {
                #ifdef ITA
                std::string phrase ("^0BanBot:^2 correzione team ^1in corso.");
                #else
                std::string phrase ("^0BanBot: ^2 fixing teams.");
                #endif
                block->say(phrase);
                
                if ( red > blue ){
                    for( unsigned int i = 0; i < m_dati->currentServer()->size() && red > blue; i++)
                        if ( (*m_dati->currentServer())[i]->team == Server::Player::RED ) {
                            block->force((*m_dati->currentServer())[i]->number,"blue");
                            #ifdef DEBUG_MODE
                            std::cout<<"Forcing player number "<<(*m_dati->currentServer())[i]->number<<" "<<(*m_dati->currentServer())[i]->nick<<" to blue.\n";
                            #endif
                            red--;
                            blue++;
                        }
                }
                else {
                    for( unsigned int i = 0; i < m_dati->currentServer()->size() && red < blue; i++)
                        if ( (*m_dati->currentServer())[i]->team == Server::Player::BLUE ) {
                            block->force((*m_dati->currentServer())[i]->number,"red");
                            #ifdef DEBUG_MODE
                            std::cout<<"Forcing player number "<<(*m_dati->currentServer())[i]->number<<" "<<(*m_dati->currentServer())[i]->nick<<" to red.\n";
                            #endif
                            red++;
                            blue--;
                        }
                }
            } else {
                //teams not required: wtf?!?
                #ifdef ITA
                std::string phrase ("^0BanBot:^1 i team sono ^2");
                phrase.append(handyFunctions::intToString((red)));
                phrase.append(" ^1vs ^2");
                phrase.append(handyFunctions::intToString((blue)));
                phrase.append(" ^1: stfu!");
                #else
                std::string phrase ("^0BanBot:^1 teams are ^2");
                phrase.append(handyFunctions::intToString((red)));
                phrase.append(" ^1vs ^2");
                phrase.append(handyFunctions::intToString((blue)));
                phrase.append(" ^1: stfu!");
                #endif
                block->tell(phrase,numeroAdmin);
            }
        }
        else {
            #ifdef ITA
            std::string phrase ("^0BanBot: ^1Correzione teams gia' eseguita.");
            #else
            std::string phrase ("^0BanBot: ^1Teams already done.");
            #endif
            block->tell(phrase,numeroAdmin);
        }
        m_scheduler->addInstructionBlock( block, Server::LOW );
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
        std::string phrase( "^0BanBot: ^1kick per il giocatore numero " );
        phrase.append(numero);
        phrase.append( ", " );
        phrase.append( nick );
        if ( !reason.empty() )
        {
            phrase.append( "^1 bannato per " );
            phrase.append( reason );
        }
    #else
        std::cout<<"  [+] kick due to ban.\n";
        *(m_dati->log)<<"  [+] kick due to ban.\n";
        std::string phrase( "^0BanBot: ^1kicking player number " );
        phrase.append(numero);
        phrase.append( ", " );
        phrase.append( nick );
        if ( !reason.empty() )
        {
            phrase.append( "^1 banned for " );
            phrase.append( reason );
        }
    #endif
    phrase.append( "." );
    block->say( phrase );
    block->kick( numero );
    m_scheduler->addInstructionBlock( block , Server::HIGH );
}

Analyzer::CheckTimingEnum Analyzer::checkTiming ( const std::vector<Db::idMotiveStruct> &records, const Server::Timing &option )
{
    // depending on actual time, time of ban, and options, i decide if the player has to be kicked, or if advice admins.
    if ( records.size() )
    {
        if ( option != Server::NEVER && records.size() )
        {
            if ( option == Server::ALWAYS ) return Analyzer::BANNED;
            
            std::string ora;
            std::string data;
            getDateAndTime(data,ora);
            
            if (data.compare(records[0].date) == 0)
            {
            
                int oraAttuale=atoi(ora.substr(0,2).c_str());
                int minutoAttuale=atoi(ora.substr(3,5).c_str());
                int oraBan = atoi(records[0].time.substr(0,2).c_str());
                int minutoBan = atoi(records[0].time.substr(3,5).c_str());
                int diff = (oraAttuale*60+minutoAttuale)-(oraBan*60+minutoBan);
                
                //i check options to see if i have to ban or advice.
                if ( (diff < 30 && option == Server::TIRTEEN) || (diff < 15 && option == Server::FIFTEEN) || (diff < 10 && option == Server::TEN) ||
                    diff < 5 ) return Analyzer::BANNED;
            }
        }
        //if i arrived at this point, player is in db like banned player, but he hasn't to be kicked. I'll check if i have to show a warning.
        if ( (*m_dati)[m_dati->serverNumber].banWarnings() )
            return Analyzer::WARNING;
        else return Analyzer::NOPE;
    }
    return Analyzer::NOPE;
}

bool Analyzer::guidIsBanned(const std::string &guid, const std::string &nick, const std::string &numero, const std::string &ip)
{
    if ( (*m_dati)[m_dati->serverNumber].strict() > LEVEL0 )
    {
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
    }
    return false;
}

bool Analyzer::nickIsBanned(const std::string &nick, const std::string &numero, const std::string &ip, const std::string &guid)
{

    if ( (*m_dati)[m_dati->serverNumber].strict() > LEVEL0 )
    {
        std::vector<Db::idMotiveStruct> risultato = database->idMotiveViaNick(correggi(nick));
        std::string ora;
        std::string data;
        getDateAndTime(data,ora);
        
        Analyzer::CheckTimingEnum ris = checkTiming( risultato, (*m_dati)[m_dati->serverNumber].banNick() );

        if( ris == Analyzer::BANNED )
        {
            #ifdef ITA
            std::cout<<"  [!] kick a "<<nick<<" per ban sul nick ("<<risultato[0].motive<<").\n";
            *(m_dati->log)<<"  [!] kick a "<<nick<<" per ban sul nick ("<<risultato[0].motive<<").\n";
            #else
            std::cout<<"  [!] kicking "<<nick<<" for banned nick ("<<risultato[0].motive<<").\n";
            *(m_dati->log)<<"  [!] kicking "<<nick<<" for banned nick ("<<risultato[0].motive<<").\n";
            #endif
            #ifdef DEBUG_MODE
            std::cout<<"  Strict level was "<<(*m_dati)[m_dati->serverNumber].strict()<<".\n";
            #endif
            buttaFuori(risultato[0].motive, numero, nick);

            std::string ora;
            std::string data;
            getDateAndTime(data,ora);
            database->modifyBanned(std::string(),ip,data,ora,std::string(),risultato[0].id);
            database->insertNewGuid(correggi(guid),risultato[0].id);
            return true;
        }
        else if ( ris == Analyzer::WARNING )
        {
            std::string message("^0BanBot: ^1");
            #ifdef ITA
                message.append("attenzione, il nick '^2");
                message.append(nick);
                message.append("^1' e' tra quelli bannati.");
            #else
                message.append("warning, the nick '^2");
                message.append(nick);
                message.append("^1' is into banned list.");
            #endif
            //already checked if warnings are disabled
            if ( (*m_dati)[m_dati->serverNumber].banWarnings() == Server::PRIVATE )
                tellToAdmins(message);
            else {
                InstructionsBlock *block=new InstructionsBlock();
                block->say(message);
                m_scheduler->addInstructionBlock(block, Server::LOW);
            }
        }
    }
    return false;
}

bool Analyzer::ipIsBanned(const std::string &ip, const std::string &numero, const std::string &nick, const std::string &guid)
{

    if ( (*m_dati)[m_dati->serverNumber].strict() > LEVEL0 )
    {
        std::vector<Db::idMotiveStruct> risultato = database->idMotiveViaIp( ip );

        Analyzer::CheckTimingEnum ris = checkTiming( risultato, (*m_dati)[m_dati->serverNumber].banIp() );
        
        if( ris == Analyzer::BANNED )
        {
            #ifdef ITA
            std::cout<<"  [!] kick a "<<nick<<" per ban sull'ip ("<<risultato[0].motive<<").\n";
            *(m_dati->log)<<"  [!] kick a "<<nick<<" per ban sull'ip ("<<risultato[0].motive<<").\n";
            #else
            std::cout<<"  [!] kicking "<<nick<<" for banned ip ("<<risultato[0].motive<<").\n";
            *(m_dati->log)<<"  [!] kicking "<<nick<<" for banned ip ("<<risultato[0].motive<<").\n";
            #endif
            #ifdef DEBUG_MODE
            std::cout<<"  Strict level was "<<(*m_dati)[m_dati->serverNumber].strict()<<".\n";
            #endif
            buttaFuori(risultato[0].motive, numero, nick);
            
            std::string ora;
            std::string data;
            getDateAndTime(data,ora);
            database->modifyBanned(std::string(),ip,data,ora,std::string(),risultato[0].id);
            database->insertNewGuid(correggi(guid),risultato[0].id);
            return true;
        }
        else if ( ris == Analyzer::WARNING )
        {
            std::string message("^0BanBot: ^1");
            #ifdef ITA
            message.append("attenzione, l'ip del player '^2");
            message.append(nick);
            message.append("^1' e' tra quelli bannati.");
            #else
            message.append("warning, the ip of '^2");
            message.append(nick);
            message.append("^1' is into banned list.");
            #endif
            //already checked if warnings are disabled
            if ( (*m_dati)[m_dati->serverNumber].banWarnings() == Server::PRIVATE )
                tellToAdmins(message);
            else {
                InstructionsBlock *block=new InstructionsBlock();
                block->say(message);
                m_scheduler->addInstructionBlock(block, Server::LOW);
            }
        }
    }
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
        if (database->checkAuthGuid(correggi((*m_dati)[m_dati->serverNumber][i]->GUID))<100)
            temp.push_back(i);
    }
    return temp;
}

void Analyzer::tellToAdmins(std::string phrase)
{
    if ( (*m_dati)[m_dati->serverNumber].warnings() != Server::DISABLED )
    {
        InstructionsBlock * block = new InstructionsBlock();
        if ( (*m_dati)[m_dati->serverNumber].warnings() == Server::PRIVATE )
        {
            std::vector<unsigned int> indici=admins();
            for (unsigned int i=0;i<indici.size();i++)
            {
                block->tell(phrase,(*m_dati)[m_dati->serverNumber][indici[i]]->number);
            }

        }
        else
        {
            block->say(phrase);
        }
        m_scheduler->addInstructionBlock( block , Server::LOW );
    }
}

int Analyzer::translatePlayer(std::string player)
{
    bool unique=true;
    int index=-1;
    for (unsigned int i=0;(i<(*m_dati)[m_dati->serverNumber].size() && unique);i++)
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

int Analyzer::translateMap(std::string map, std::string map2)
{
    int index=-1;
    std::string regex("[^ \t\n\r\f\v]*");
    regex.append(map);
    regex.append("[^ \t\n\r\f\v]*");
    if (!map2.empty()){
        regex.append(map2);
        regex.append("[^ \t\n\r\f\v]*");
    }
    std::vector<std::string> maps=(m_dati->currentServer())->serverMaps();
    for (unsigned int i=0;index<0 && i<maps.size();i++)
    {
        if( isA(maps.at(i).c_str(),regex) ) index=i;
    }
    return index;
}

int Analyzer::translateConfig(std::string conf, std::string conf2)
{
    int index=-1;
    std::string regex("[^ \t\n\r\f\v]*");
    regex.append(conf);
    regex.append("[^ \t\n\r\f\v]*");
    if (!conf2.empty()){
        regex.append(conf2);
        regex.append("[^ \t\n\r\f\v]*");
    }
    std::vector<std::string> confs=(m_dati->currentServer())->serverConfigs();
    for (unsigned int i=0;index<0 && i<confs.size();i++)
    {
        if( isA(confs.at(i).c_str(),regex) ) index=i;
    }
    return index;
}

int Analyzer::findPlayer( std::string number )
{
    bool nonTrovato = true;
    int index = -1;

    for (unsigned int i=0; nonTrovato && ( i<(*m_dati)[m_dati->serverNumber].size() ); i++)
    {
        if ((*m_dati)[m_dati->serverNumber][i]->number.compare(number) == 0)
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
    #ifdef ITA
        std::cout<<"\e[0;32m[OK] BanBot avviato. \e[0m \n\n";
        *(m_dati->errors)<<"[OK] BanBot avviato.\n\n";
    #else
        std::cout<<"\e[0;32m[OK] BanBot launched. \e[0m \n\n";
        *(m_dati->errors)<<"[OK] BanBot launched.\n\n";
    #endif
    
    while (true)
    {
        commandexecuted=false;
        //vedo è il caso di fare il backup
        if(backup->doJobs())
        {
            server->sendInfo();
            //backup done, reload of servers,
            for (m_dati->serverNumber = 0; m_dati->serverNumber < m_dati->size(); m_dati->serverNumber++)
            {
                InstructionsBlock * block = new InstructionsBlock();
                #ifdef ITA
                block->say("^0BanBot: ^2ATTENZIONE: ^1inizio backup, il server verra' riavviato a breve.");
                #else
                block->say("^0BanBot: ^2WARNING: ^1starting backup, this server will be restarted shortly.");
                #endif
                m_scheduler->addInstructionBlock( block, Server::HIGH );
            }
            while (m_scheduler->executeInstructions()){}
            sleep(2);
            for (m_dati->serverNumber = 0; m_dati->serverNumber < m_dati->size(); m_dati->serverNumber++)
            {
                InstructionsBlock * block = new InstructionsBlock();
                block->reload();
                m_scheduler->addInstructionBlock( block, Server::HIGH );
            }
            //if the backup gone successfully, i'll reset the log line pointer.
            for (unsigned int i=0;i<m_dati->size();i++)
            {
                log=new std::ifstream();
                log->open((*m_dati)[i].botLog().c_str());
                if (log->is_open())
                {
                    //if the file is open i'll check his dimension
                    log->seekg (0, std::ios::end);
                    if (log->tellg()<(*m_dati)[i].row()) (*m_dati)[i].setRow(0);
                }
                //if the file is not found, just reset the counter.
                else (*m_dati)[i].setRow(0);
                log->close();
                delete log;
                //log was interrupted: i'll reset player, eventually re-catched when i'll start to analyze the log again.
                for (unsigned int j=0;j<(*m_dati)[i].size();j++) delete (*m_dati)[i][j];
                //resetto il vector:
                (*m_dati)[i].clear();
            }
            m_dati->serverNumber=0;
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
                loadOptions(); //it does updateServerConfigMapList too.
            }
            else m_fileLister->updateServerConfigMapList();
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
                log=new std::ifstream();
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

                        if (isA(line, _R_CLIENT_USER_INFO))
                        {
                            //it is a clientUserinfo line
                            clientUserInfo(line);
                            commandexecuted=true;
                        }
                        else
                        {
                            if (isA(line, _R_CLIENT_USER_INFO_CHANGED))
                            {
                                clientUserInfoChanged(line);
                            }
                            else
                            {
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
                                                if (isA(line, _R_STATUS))
                                                    status(line);
                                                else if (isA(line, _R_STRICT))
                                                    setStrict(line);
                                                else if (isA(line, _R_IAMGOD))
                                                    iamgod(line);
                                                else if ( m_dati->currentServer()->strict() > LEVEL0 ){
                                                    if (isA(line, _R_BAN))
                                                        ban(line);
                                                    else if (isA(line,_R_FIND))
                                                        find(line);
                                                    else if (isA(line, _R_UNBAN)) 
                                                        unban(line);
                                                    else if (isA(line, _R_OP))
                                                        op(line);
                                                    else if (isA(line, _R_DEOP)) 
                                                        deop(line);
                                                    else if (isA(line, _R_HELP))
                                                        help(line);
                                                    else if (isA(line, _R_FINDOP))
                                                        findOp(line);
                                                    else if (isA(line, _R_KICK))
                                                        kick(line);
                                                    else if (isA(line, _R_MUTE))
                                                        mute(line);
                                                    else if (isA(line, _R_VETO))
                                                        veto(line);
                                                    else if (isA(line, _R_SLAP))
                                                        slap(line);
                                                    else if (isA(line, _R_NEXTMAP))
                                                        nextmap(line);
                                                    else if (isA(line, _R_MAP))
                                                        map(line);
                                                    else if (isA(line, _R_FORCE))
                                                        force(line);
                                                    else if (isA(line, _R_NUKE))
                                                        nuke(line);
                                                    else if (isA(line, _R_ADMINS))
                                                        admins(line);
                                                    else if (isA(line, _R_EMPTY_PASS))
                                                        pass(line);
                                                    else if (isA(line, _R_CONFIG))
                                                        config(line);
                                                    else if (isA(line, _R_WARNINGS))
                                                        warnings(line);
                                                    else if (isA(line, _R_BANTIMEWARN))
                                                        bantimewarn(line);
                                                    else if (isA(line, _R_RESTART))
                                                        restart(line);
                                                    else if (isA(line, _R_RELOAD))
                                                        reload(line);
                                                    else if (isA(line, _R_BALANCE))
                                                        balance(line);
                                                    else if (isA(line, _R_GRAVITY))
                                                        gravity(line);
                                                    else if (isA(line, _R_CHANGELEVEL))
                                                        changeLevel(line);
                                                    else if (isA(line, _R_BIGTEXT))
                                                        bigtext(line);
                                                    else if (isA(line, _R_TEAMS))
                                                        teams(line);
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
        //end of servers, wait some time.
        if ( commandexecuted )
        {
            fascia=0;
            contatore=0;
        }
        else contatore++;
        if ( (fascia == 0 && contatore > 30) || (fascia == 1 && contatore > 60) )
        {
            fascia++;
            contatore++;
        }

        if ( !m_scheduler->executeInstructions() )
        {
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
}
#endif
