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


Scheduler::Scheduler( ConfigLoader::Options *op, Connection *conn )
    : m_connection( conn )
    , m_options( op )
{
}

Scheduler::~Scheduler()
{
}

void Scheduler::addInstructionBlock( InstructionsBlock* inst, Server::PriorityLevel lvl )
{
    m_options->currentServer()->setPriorityInstrBlock( lvl, inst );
}

bool Scheduler::executeInstructions()
{
    bool executedInstr = false;     // bool to check if i've executed at least one instruction

    for( unsigned int i = 0; i < m_options->size(); i++ ) {
        Server *auxServer = m_options->servers.at( i );
        Server::InstructionCounter *auxInstCounter = auxServer->instructionCounter();

        // get various instruction block levels
        InstructionsBlock *auxHighPr = auxServer->priorityInstrBlock( Server::HIGH );
        InstructionsBlock *auxMedPr = auxServer->priorityInstrBlock( Server::MEDIUM );
        InstructionsBlock *auxLowPr = auxServer->priorityInstrBlock( Server::LOW );

        for( int j = 0; j < 3; j++ ) {
            if( auxServer->instructionCounter()->hightPr() < MAX_HIGH_INST )   // check i haven't exceeded max counter
                executedInstr = executePriorityInstruction( auxHighPr, auxInstCounter, Server::HIGH, i );
            else if( auxServer->instructionCounter()->medPr() < MAX_MED_INST )
                executedInstr = executePriorityInstruction( auxMedPr, auxInstCounter, Server::MEDIUM, i );
            else
                if( auxServer->instructionCounter()->lowPr() < MAX_LOW_INST )
                    executedInstr = executePriorityInstruction( auxLowPr, auxInstCounter, Server::LOW, i );
        }
        usleep( 600000 );   // 0,6 sec
    }
    return executedInstr;
}

bool Scheduler::executePriorityInstruction( InstructionsBlock* instr, Server::InstructionCounter *counter, Server::PriorityLevel lvl, int serverNum )
{
    if( instr != 0 ) {
        // "execFirstCommand" deletes the instruction or removes it or does something to it so
        // i don't have to touch anything inside
        instr->execFirstCommand( m_connection, serverNum );     // execute command
        counter->incrementPriority( lvl );                      // increment counter of instruction level

        if( instr->isEmpty() ) {
            InstructionsBlock *tmp = instr;
            instr = instr->getNext();
            delete tmp;
        }
        else
            instr->moveToTail();

        if( lvl == Server::LOW )                                // if I execute a low level instruction, I have to reset counters
            counter->resetCounters();

        return true;
    }
    return false;
}
