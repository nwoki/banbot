/*
    ConfigLoader.cpp is part of BanBot.

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

#ifndef _ConfigLoader_cpp_
#define _ConfigLoader_cpp_

#include <regex.h>
#include <stdlib.h>
#include "ConfigLoader.h"

//costruttore: apro il file
ConfigLoader::ConfigLoader( const std::string &filePath )
{
    generalFile=filePath;
    opzioni=new Options();
    loadOptions();
}

//distruttore:chiudo il file
ConfigLoader::~ConfigLoader()
{
    if ( opzioni != NULL )
        delete opzioni;
}

ConfigLoader::Options* ConfigLoader::getOptions()
{
  return opzioni;
}

bool ConfigLoader::testChanges()
{
    if (opzioni != NULL)
    {
        struct stat st;
        if( stat( generalFile.c_str(), &st ) == 0 )
        {
            if(difftime(st.st_mtime, (opzioni->infos).st_mtime) > 0)
            {
                //il file generale è cambiato
                return true;
            }
        }
        for (unsigned int i=0; i<opzioni->size(); i++)
        {
            if( !(*opzioni)[i].configFile().empty() && stat( (*opzioni)[i].configFile().c_str(), &st ) == 0 )
            {
                if(difftime(st.st_mtime, (*opzioni)[i].infos().st_mtime) > 0)
                {
                    //il file del server è cambiato
                    return true;
                }
            }
        }
    }
    return false;
}

void ConfigLoader::loadOptions()
{
    if( stat( generalFile.c_str(), &(opzioni->infos) ) == 0 )
    {
        #ifdef DEBUG_MODE
        std::cout << "Inizio il caricamento delle opzioni...\n";
        #endif
        std::ifstream * cfg=new std::ifstream();
        cfg->open( generalFile.c_str() );
        //leggo il file fino alla fine
        if( cfg->is_open() )
        {
            Options* newOptions = new Options;
            //Prendo tutto fino al primo server (o fine file)
            char line [3000]; //used for the main config file
            cfg->getline( line, 3000, '{' );

            //prendo i parametri generali
            std::string all( line );
            unsigned int pos = 0;
            unsigned int end = 0;

            while(  end < all.size() && pos >= end  )
            {
                end = all.find( '\n', pos );
                std::string temp ( all.substr( pos,end-pos ) );
                pos = end+1;

                #ifdef DEBUG_MODE
                std::cout << temp << "\n";
                #endif
                if ( !isA( temp, (char*)" *#") && isA( temp, (char*)"^[ \t]*GENERAL_[^ \t\n\r\f\v]+[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                {
                    if ( isA( temp, (char*)"^[ \t]*GENERAL_BOTLOG[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                    {
                        newOptions->generalLog = extract ( temp );
                        #ifdef DEBUG_MODE
                        std::cout << "General_Botlog catturato: " << newOptions->generalLog << "\n";
                        #endif
                    }
                    else if ( isA( temp, (char*)"^[ \t]*GENERAL_BACKUP_PATH[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                    {
                        newOptions->generalBackup = extract ( temp );
                        #ifdef DEBUG_MODE
                        std::cout << "General_Backup_path catturato: " << newOptions->generalBackup << "\n";
                        #endif
                    }
                    else if ( isA( temp, (char*)"^[ \t]*GENERAL_PORT[ \t]*=[ \t]*[0-9]{1,5}$" ) )
                    {
                        newOptions->generalPort = atoi( extract ( temp ).c_str() );
                        #ifdef DEBUG_MODE
                        std::cout << "General_port catturato: " << newOptions->generalPort << "\n";
                        #endif
                    }
                    else
                    {
                        #ifdef ITA
                            std::cout << "Opzione non valida: " << temp << "\n";
                        #else
                            std::cout << "Option not valid: " << temp << "\n";
                        #endif
                    }
                }
            }

            //if i have all general parameters ok, i can go, else i'll stop, 'cause options are wrong.
            #ifdef DEBUG_MODE
            std::cout << "Caricate le opzioni generali, inizio con quelle dei server.\n";
            #endif

            if ( !newOptions->generalBackup.empty() && !newOptions->generalLog.empty() )
            {
                //vado a prendere tutto il resto
                while( !cfg->eof() )
                {
                    cfg->getline( line, 3000, '}' );

                    if( !cfg->eof() )
                    {
                        //dentro a line ho tutte le opzioni di un singolo server, uso i regex per riconoscere le opzioni.

                        Server* newServer = new Server();
                        all.clear();
                        all = std::string(line);
                        pos = 0;
                        end = 0;
                        bool secondary = false;

                        #ifdef DEBUG_MODE
                        std::cout << "Inizio il server...\n";
                        #endif

                        while( end < all.size() && pos >= end )
                        {
                            end = all.find('\n',pos);
                            std::string temp ( all.substr( pos,end-pos ) );

                            #ifdef DEBUG_MODE
                            std::cout << temp << "\n";
                            #endif
                            pos=end+1;

                            if ( !isA( temp,(char*) "[ \t]*#") && isA( temp, (char*)"^[ \t]*[^ \t\n\r\f\v]+[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                            {
                                //if it isn't a comment or shit....
                                if ( isA (temp, (char *)"^[ \t]*LEVEL_" ) )
                                {
                                    //if it is a level power change
                                    if ( isA (temp, (char *)"^[ \t]*LEVEL_HELP[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::HELP, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_KICK[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::KICK, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_BAN[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::BAN, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_UNBAN[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::UNBAN, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_MUTE[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::MUTE, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_OP[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::OP, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_DEOP[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::DEOP, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_STRICT[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::STRICT, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_NUKE[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::NUKE, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_SLAP[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::SLAP, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_VETO[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::VETO, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_FORCE[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::FORCE, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_MAP[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::MAP, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_NEXTMAP[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::NEXTMAP, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_ADMINS[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::ADMINS, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_PASS[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::PASS, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_CONFIG[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::CONFIG, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_STATUS[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::STATUS, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_WARNINGS[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::WARNINGS, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_BANTIMEWARN[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::BANTIMEWARN, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_RESTART[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::RESTART, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_RELOAD[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::RELOAD, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_TEAMBALANCE[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::TEAMBALANCE, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_GRAVITY[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::GRAVITY, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_CHANGELEVEL[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::CHANGELEVEL, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_BIGTEXT[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::BIGTEXT, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_TEAMS[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::TEAMS, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_PLAYERSINFO[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                        newServer->setCommandPermission( Server::PLAYERSINFO, atoi(extract(temp).c_str()));
                                    else if ( isA (temp, (char *)"^[ \t]*LEVEL_CYCLEMAP[ \t]*=[ \t]*[0-9]{1,3}$" ) )
                                    newServer->setCommandPermission( Server::CYCLE, atoi(extract(temp).c_str()));
                                    else {
                                        #ifdef ITA
                                        std::cout << "Attenzione: \"" << temp << "\" non e' un'opzione valida!.\n";
                                        if ( opzioni->errors )
                                        {
                                            opzioni->errors->timestamp();
                                            *(opzioni->errors) << "\nAttenzione: \"" << temp << "\" non e' un'opzione valida!.\n";
                                            opzioni->errors->close();
                                        }
                                        #else
                                        std::cout << "Warning: \"" << temp << "\" isn't a valid option.\n";
                                        if ( opzioni->errors )
                                        {
                                            opzioni->errors->timestamp();
                                            *(opzioni->errors) << "\nWarning: \"" << temp << "\" isn't a valid option.\n";
                                            opzioni->errors->close();
                                        }
                                        #endif
                                    }
                                }
                                else if ( isA( temp, (char *)"^[ \t]*SERVER_NAME[ \t]*=[ \t]*[^ \t\n\r\f\v]+$" ) )
                                    newServer->setName( extract( temp ) );
                                else if ( isA( temp, (char*)"^[ \t]*IP[ \t]*=[ \t]*[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}$" ) )
                                    newServer->setIP( extract( temp ) );
                                else if ( isA( temp, (char*)"^[ \t]*PORT[ \t]*=[ \t]*[0-9]{1,5}$" ) )
                                    newServer->setPort( atoi( extract( temp ).c_str() ) );
                                else if ( isA( temp, (char*)"^[ \t]*WARNINGS[ \t]*=[ \t]*(off|private|public)$" ) )
                                {
                                    if ( extract( temp ).compare("off") == 0 ) newServer->setWarnings( Server::DISABLED );
                                    else if ( extract( temp ).compare("private") == 0 ) newServer->setWarnings( Server::PRIVATE );
                                    else  newServer->setWarnings( Server::PUBLIC );
                                }
                                else if ( isA( temp, (char*)"^[ \t]*BAN_WARNINGS[ \t]*=[ \t]*(off|on)$" ) )
                                {
                                    if ( extract( temp ).compare("off") == 0 ) newServer->setBanWarnings( false );
                                    else  newServer->setBanWarnings( true );
                                }
                                else if ( isA( temp, (char*)"^[ \t]*NICK_TIMING[ \t]*=[ \t]*(never|5|10|15|30|always)$" ) )
                                {
                                    std::string opt = extract( temp );
                                    if ( opt.compare("never") == 0 ) newServer->setBanNick( Server::NEVER );
                                    else if ( opt.compare("5") == 0 ) newServer->setBanNick( Server::FIVE );
                                    else if ( opt.compare("10") == 0 ) newServer->setBanNick( Server::TEN );
                                    else if ( opt.compare("15") == 0 ) newServer->setBanNick( Server::FIFTEEN );
                                    else if ( opt.compare("30") == 0 ) newServer->setBanNick( Server::TIRTEEN );
                                    else  newServer->setBanNick( Server::ALWAYS );
                                }
                                else if ( isA( temp, (char*)"^[ \t]*IP_TIMING[ \t]*=[ \t]*(never|5|10|15|30|always)$" ) )
                                {
                                    std::string opt = extract( temp );
                                    if ( opt.compare("never") == 0 ) newServer->setBanIp( Server::NEVER );
                                    else if ( opt.compare("5") == 0 ) newServer->setBanIp( Server::FIVE );
                                    else if ( opt.compare("10") == 0 ) newServer->setBanIp( Server::TEN );
                                    else if ( opt.compare("15") == 0 ) newServer->setBanIp( Server::FIFTEEN );
                                    else if ( opt.compare("30") == 0 ) newServer->setBanIp( Server::TIRTEEN );
                                    else  newServer->setBanIp( Server::ALWAYS );
                                }
                                else if ( isA( temp, (char*)"^[ \t]*RCON_PASSWORD[ \t]*=[ \t]*[^ \t\n\r\f\v]+$" ) )
                                    newServer->setRcon( extract( temp ) );

                                else if ( isA( temp, (char*)"^[ \t]*GAME_LOGFILE[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                                {
                                    std::string t = extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setServerLog( t );
                                }

                                else if ( isA( temp, (char*)"^[ \t]*BOT_LOGFILE[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                                {
                                    std::string t = extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setBotLog( t );
                                }

                                else if ( isA( temp, (char*)"^[ \t]*BACKUP_DIR[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                                {
                                    std::string t=extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setBackupDir( t );
                                }

                                else if ( isA( temp, (char*)"^[ \t]*GAME_DIR[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                                {
                                    std::string t=extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setGameDirectory( t );
                                }

                                else if ( isA( temp, (char*)"^[ \t]*DATABASE_DIR[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) )
                                {
                                    std::string t=extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setDbFolder( t );
                                }

                                else if ( isA( temp, (char*)"^[ \t]*STRICT_LEVEL[ \t]*=[ \t]*[0-4]{1}$" ) )
                                    newServer->setStrict( atoi( extract( temp ).c_str() ) );
                                    
                                else if ( isA( temp, (char*)"^[ \t]*MESSAGE[ \t]*=[ \t]*[^\t\n\r\f\v]+$" ) )
                                    newServer->addSpamMessage( extract( temp ) );
                                
                                else if ( isA( temp, (char*)"^[ \t]*ACCEPT_QUAKE_CLIENTS[ \t]*=[ \t]*(YES|yes|NO|no){1}$" ) )
                                {
                                    std::string opt = extract( temp );
                                    if ( opt.compare("no") == 0 || opt.compare("NO") == 0 ) newServer->setAcceptQuakeClients(false);
                                    else newServer->setAcceptQuakeClients(true);
                                }

                                else if ( temp.compare("EXTERNAL_OPTIONS = YES") == 0 )
                                    secondary = true;

                                else if ( isA( temp, (char*)"^[ \t]*CONFIG_FILE[ \t]*=[ \t]*[^\t\n\r\f\v]+[^ \t\n\r\f\v]$" ) && !secondary )
                                {
                                    newServer->setConfigFile( extract(temp) );
                                    struct stat info;
                                    if( stat( newServer->configFile().c_str(), &info ) == 0 )
                                    {
                                        char extra [2000]; //used for secondary config files.
                                        newServer->setInfos( info );
                                        std::ifstream* secondary=new std::ifstream();
                                        secondary->open( newServer->configFile().c_str() );
                                        secondary->getline( extra, 2000, '}' );
                                        //appendo il "segnale" per indicare le opzioni sul file secondario
                                        all.append( "EXTERNAL_OPTIONS = YES\n" );
                                        //appendo le opzioni del file secondario
                                        all.append( extra );
                                        secondary->close();
                                        delete secondary;
                                    }
                                    else
                                    {
                                        #ifdef ITA
                                            std::cout << "[FAIL]: non riesco ad aprire \"" << newServer->configFile() << "\".\n";
                                            if ( opzioni->errors )
                                            {
                                                opzioni->errors->timestamp();
                                                *(opzioni->errors) << "[FAIL]: non riesco ad aprire \"" << newServer->configFile() << "\".\n";
                                                opzioni->errors->close();
                                            }
                                        #else
                                            std::cout << "[FAIL]: i can't open \"" << newServer->configFile() << "\".\n";
                                            if ( opzioni->errors )
                                            {
                                                opzioni->errors->timestamp();
                                                *(opzioni->errors) << "[FAIL]: i can't open \"" << newServer->configFile() << "\".\n";
                                                opzioni->errors->close();
                                            }
                                        #endif
                                    }
                                }
                                else
                                {
                                    #ifdef ITA
                                        std::cout << "Attenzione: \"" << temp << "\" non e' un'opzione valida!.\n";
                                        if ( opzioni->errors )
                                        {
                                            opzioni->errors->timestamp();
                                            *(opzioni->errors) << "\nAttenzione: \"" << temp << "\" non e' un'opzione valida!.\n";
                                            opzioni->errors->close();
                                        }
                                    #else
                                        std::cout << "Warning: \"" << temp << "\" isn't a valid option.\n";
                                        if ( opzioni->errors )
                                        {
                                            opzioni->errors->timestamp();
                                            *(opzioni->errors) << "\nWarning: \"" << temp << "\" isn't a valid option.\n";
                                            opzioni->errors->close();
                                        }
                                    #endif
                                }
                            }
                        }
                        //i'll add the server if it is ok, else fu.
                        if ( newServer->test_for_options() )
                            newOptions->servers.push_back( newServer );
                        else
                        {
                            #ifdef ITA
                                std::cout<<"Server ignorato per opzioni non valide o mancanti.\n";
                            #else
                                std::cout<<"Server ignored due to invalid or missed options.\n";
                            #endif
                            delete newServer;
                        }

                        #ifdef DEBUG_MODE
                            std::cout << "Finito il server\n";
                        #endif
                    }
                }

                //ok, options loaded.
                //if this is a change, i look for previouse servers parameters (like players in-game)
                for ( unsigned int i = 0; i < newOptions->size(); i++ )
                    (*newOptions)[i].test_for_changes( opzioni->searchServer( (*newOptions)[i].name() ) );

                opzioni->destroyServers();
                opzioni->servers=newOptions->servers;
                delete opzioni->errors;
                opzioni->errors = new Logger( newOptions->generalLog );
                opzioni->generalLog = newOptions->generalLog;
                opzioni->generalBackup = newOptions->generalBackup;
                opzioni->generalPort = newOptions->generalPort;


                //ok, sistemato il trasferimento di parametri da vecchi a nuovi ^^ elimino gli oggetti temporanei
                delete newOptions;

                #ifdef DEBUG_MODE
                    std::cout << "Finito il caricamento delle opzioni.\n";
                #endif
            }
            else
            {
                //errore nelle impostazioni generali
                #ifdef ITA
                    std::cout << "[FAIL]: ho rilevato degli errori nelle opzioni generali. Ignoro le nuove opzioni.\n";
                    if ( opzioni->errors )
                    {
                        opzioni->errors->timestamp();
                        *(opzioni->errors) << "\n[FAIL]: ho rilevato degli errori nelle opzioni generali. Ignoro le nuove opzioni.\n";
                        opzioni->errors->close();
                    }
                #else
                    std::cout << "[FAIL]: errors detected in general options. Ignoring new options\n";
                    if ( opzioni->errors )
                    {
                        opzioni->errors->timestamp();
                        *(opzioni->errors) << "\n[FAIL]: errors detected in general options. Ignoring new options\n";
                        opzioni->errors->close();
                    }
                #endif
            }
        }
        cfg->close();
        delete cfg;
    }
}

std::string ConfigLoader::extract(const std::string &line)
{
    int pos = line.find( '=' );
    pos = line.find_first_not_of( " \t", pos+1 );
    return line.substr( pos );
}

bool ConfigLoader::isA( const std::string &line, char* regex )
{
    regex_t r;

    if ( regcomp( &r, regex, REG_EXTENDED|REG_NOSUB ) == 0) {
        int status = regexec( &r, line.c_str(), ( size_t )0, NULL, 0 );
        regfree( &r );
        if( status == 0 )
            return true;
    }
    return false;
}
#endif
