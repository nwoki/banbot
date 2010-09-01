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

    Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


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
    if ( opzioni != 0 )
        delete opzioni;
}

ConfigLoader::Options* ConfigLoader::getOptions()
{
  return opzioni;
}

bool ConfigLoader::testChanges()
{
    if (opzioni)
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
            char line [3000];
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
                if ( !isA( temp, (char*)" *#") && isA( temp, (char*)" **GENERAL_[^ \t\n\r\f\v]+ *= *[^ \t\n\r\f\v]+" ) )
                {
                    if ( isA( temp, (char*)" *GENERAL_BOTLOG *= *[^ \t\n\r\f\v]+" ) )
                    {
                        newOptions->generalLog = extract ( temp );
                        #ifdef DEBUG_MODE
                        std::cout << "General_Botlog catturato: " << newOptions->generalLog << "\n";
                        #endif
                    }

                    if ( isA( temp, (char*)" *GENERAL_BACKUP_PATH *= *[^ \t\n\r\f\v]+" ) )
                    {
                        newOptions->generalBackup = extract ( temp );
                        #ifdef DEBUG_MODE
                        std::cout << "General_Backup_path catturato: " << newOptions->generalBackup << "\n";
                        #endif
                    }
                }
            }

            //se sono stati impostati tutti i parametri generali, proseguo, altrimenti me ne strafrego perchè le impostazioni sono già sbagliate
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

                        all = line;
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
                            
                            if ( !isA( temp,(char*) " *#") && isA( temp, (char*)" *[^ \t\n\r\f\v]+ *= *[^ \t\n\r\f\v]+" ) )
                            {
                                //se non è un commento o una puttanata....
                                if ( isA( temp, (char *)" *SERVER_NAME *= *[^ \t\n\r\f\v]+" ) )
                                    newServer->setName( extract( temp ) );
                                else if ( isA( temp, (char*)" *IP *= *[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}" ) )
                                    newServer->setIP( extract( temp ) );
                                else if ( isA( temp, (char*)" *PORT *= *[0-9]{1,5}" ) )
                                    newServer->setPort( atoi( extract( temp ).c_str() ) );

                                else if ( isA( temp, (char*)" *RCON_PASSWORD *= *[^ \t\n\r\f\v]+" ) )
                                    newServer->setRcon( extract( temp ) );

                                else if ( isA( temp, (char*)" *GAME_LOGFILE *= *[^ \t\n\r\f\v]+" ) )
                                {
                                    std::string t = extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setServerLog( t );
                                }

                                else if ( isA( temp, (char*)" *BOT_LOGFILE *= *[^ \t\n\r\f\v]+" ) )
                                {
                                    std::string t = extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setBotLog( t );
                                }

                                else if ( isA( temp, (char*)" *BACKUP_DIR *= *[^ \t\n\r\f\v]+" ) )
                                {
                                    std::string t=extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setBackupDir( t );
                                }

                                else if ( isA( temp, (char*)" *DATABASE_DIR *= *[^ \t\n\r\f\v]+" ) )
                                {
                                    std::string t=extract( temp );
                                    if ( t.at(0) != '/' && secondary )
                                        t.insert( 0, newServer->configFile().substr(0,newServer->configFile().find_last_of("/")+1) );

                                    newServer->setDbFolder( t );
                                }

                                else if ( isA( temp, (char*)" *STRICT_LEVEL *= *[0-3]{1}" ) )
                                    newServer->setStrict( atoi( extract( temp ).c_str() ) );

                                else if ( temp.compare("EXTERNAL_OPTIONS = YES") == 0 )
                                    secondary = true;

                                else if ( isA( temp, (char*)" *CONFIG_FILE *= *[^ \t\n\r\f\v]+" ) && !secondary )
                                {
                                    newServer->setConfigFile( extract(temp) );
                                    struct stat info;
                                    if( stat( newServer->configFile().c_str(), &info ) == 0 )
                                    {
                                        newServer->setInfos( info );
                                        std::ifstream* secondary=new std::ifstream();
                                        secondary->open( newServer->configFile().c_str() );
                                        char extra [2000];
                                        secondary->read( extra, 2000 );
                                        //appendo il "segnale" per indicare le opzioni sul file secondario
                                        all.append( "EXTERNAL_OPTIONS = YES\n" );
                                        //appendo le opzioni del file secondario
                                        all.append( extra );
                                        secondary->close();
                                        delete secondary;
                                    }
                                    else
                                    {
                                        std::cout << "[FAIL]: i can't open \"" << newServer->configFile() << "\".\n";
                                        if ( opzioni->errors )
                                        {
                                            opzioni->errors->timestamp();
                                            *(opzioni->errors) << "[FAIL]: i can't open \"" << newServer->configFile() << "\".\n";
                                            opzioni->errors->close();
                                        }
                                    }
                                }
                                else
                                {
                                    std::cout << "Warning: \"" << temp << "\" isn't a valid option.\n";
                                    if ( opzioni->errors )
                                    {
                                        opzioni->errors->timestamp();
                                        *(opzioni->errors) << "\nWarning: \"" << temp << "\" isn't a valid option.\n";
                                        opzioni->errors->close();
                                    }
                                }
                            }
                        }
                        //aggiungo il server se è ok, altrimenti lo scarto.
                        if ( newServer->test_for_options() )
                            newOptions->servers.push_back( newServer );
                        else
                            delete newServer;
                        
                        #ifdef DEBUG_MODE
                        std::cout << "Finito il server\n";
                        #endif
                    }
                }

                //ok, finito di caricare le impostazioni.
                //confronto con i server precedenti
                for ( unsigned int i = 0; i < newOptions->size(); i++ )
                    (*newOptions)[i].test_for_changes( opzioni->searchServer( (*newOptions)[i].name() ) );

                newOptions->infos = opzioni->infos;

                //ok, sistemato il trasferimento di parametri da vecchi a nuovi ^^ elimino le vecchie impostazioni.
                delete opzioni;

                //ricreo i 2 stream di log e mi salvo le impostazioni.
                newOptions->errors = new Logger( newOptions->generalLog );
                newOptions->log = new Logger();
                opzioni = newOptions;
                #ifdef DEBUG_MODE
                std::cout << "Finito il caricamento delle opzioni.\n";
                #endif
            }
            else
            {
                //errore nelle impostazioni generali
                std::cout << "[FAIL]: errors detected in general options. Ignoring new options\n";
                if ( opzioni->errors )
                {
                    opzioni->errors->timestamp();
                    *(opzioni->errors) << "\n[FAIL]: errors detected in general options. Ignoring new options\n";
                    opzioni->errors->close();
                }
            }
        }
        cfg->close();
        delete cfg;
    }
}

std::string ConfigLoader::extract(const std::string &line)
{
    int pos = line.find( '=' );
    pos = line.find_first_not_of( ' ', pos+1 );
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