/*
 *  Scheduler.cpp is part of BanBot.
 *
 *  BanBot is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  BanBot is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with BanBot (look at GPL_License.txt).
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)
 *
 *
 *  BanBot uses SQLite3:
 *  Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
 *  Software Foundation, Inc.
 */

#include "connection.h"
#include "InstructionsBlock.h"
#include "Scheduler.h"
#include "server.h"

#define MAX_HIGH_INST 3
#define MAX_MED_INST 2
#define MAX_LOW_INST 1

/// TODO make a method to check servers contained in the schedulers map that adds or deletes
/// servers if there are new ones or not. Otherwise i'll get a segfault if i go operate on a NON-existant server

Scheduler::Scheduler( ConfigLoader::Options *op, Connection *conn )
    : m_connection( conn )
    , m_options( op )
{
    // create maps with server name associated to it's counter
    for( unsigned int i = 0; i < m_options->size(); i++ ) {
        Server *auxServer = m_options->servers.at( i ); // get pointer of server to operate with
        std::map< std::string, InstructionCounter* >::value_type element( auxServer->name(), new InstructionCounter() );
        m_serverCounters.insert( element );             // map it
    }
}

Scheduler::~Scheduler()
{
}

void Scheduler::addInstructionBlock( InstructionsBlock* inst, Server::PriorityLevel lvl )
{
    m_options->currentServer()->setPriorityInstrBlock( lvl, inst );
}

/*-creare un metodo che esegue 3 istruzioni per server (3 cicli), seguendo queste regole:
 * devono essere eseguite prima le istruzioni delle liste di maggior precedenza
 * dopo 3 esecuzioni di una istruzione nella lista a priorità alta, vengono eseguite quelle della priorità normale anche se ci sono altri blocchi a priorità alta.
 * dopo 2 esecuzioni di una istruzione nella lista a priorità normale, vengono eseguite quelle della priorità bassa anche se ci sono altri blocchi a priorità normale.
 * dopo 1 esecuzione di una istruzione nella lista a bassa priorità, se ce ne sono vengono eseguite quelle a priorità alta o normale.
 * dopo aver eseguito una istruzione di un blocco, lo sposta in fondo alla coda. ( if !ins->empty() )
 * utilizza il comando usleep() con una pausa di 600000 microsecondi, ma solo se deve fare un altro giro (fa una pausa solo tra 1° e 2°, e 2° e 3° giro: in pratica se deve uscire non fa pause).
 * se non ha nulla da eseguire, esce prima.
 q uesto metodo ritorna un booleano, che sarà vaero se ha eseguito qualche istruzione, false in caso contrario.*/

bool Scheduler::executeInstructions()
{
    for( unsigned int i = 0; i < m_options->size(); i++ ) {
        Server *auxServer = m_options->servers.at( i );
        InstructionCounter *auxInstCounter = m_serverCounters[ auxServer->name() ];

        InstructionsBlock *auxHighPr = auxServer->priorityInstrBlock( Server::HIGH );

        if( auxInstCounter ) {      // got InstructionBlock, executes commands
            if( !auxInstCounter->hightPr() < MAX_HIGH_INST ) {          // check i haven't exceeded max counter
                if( !auxHighPr->isEmpty() ) {                           // check if there are instructions to execute

                    /* here i assume that "execFirstCommand" deletes the instruction or removes it or does something
                     * to it so i don't have to touch anything inside. Ask zamy how he intends to implemente InstructionsBlock */
                    auxHighPr->execFirstCommand( m_connection, i );
                    auxInstCounter->incrementHighPr();
                    if( auxHighPr->isEmpty() ) {
                        auxHighPr->setNext( auxHighPr->getNext() );     // set next instructions block
                        delete auxHighPr;                               // delete empty instructions block
                    }
                    else                                                // move instructions to tail for later execution
                        auxServer->setPriorityInstrBlock( Server::HIGH, auxHighPr->moveToTail() );
                }
            }
        }
    }
}
