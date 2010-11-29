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

    for( int j = 0; j < 3; j++ ) {
        for( unsigned int i = 0; i < m_options->size(); i++  ) {
            bool exec = true;
            if( (*m_options)[i].priorityInstrBlock(Server::HIGH) != NULL && ( (*m_options)[i].instructionCounter()->highPr() < MAX_HIGH_INST ||
                    ( (*m_options)[i].priorityInstrBlock(Server::MEDIUM) == NULL && (*m_options)[i].priorityInstrBlock(Server::LOW) == NULL ) ) )   // check i haven't exceeded max counter
                executePriorityInstruction( (*m_options)[i].priorityInstrBlock( Server::HIGH ), (*m_options)[i].instructionCounter(), Server::HIGH, i );
            else if( (*m_options)[i].priorityInstrBlock( Server::MEDIUM ) != NULL && ( (*m_options)[i].instructionCounter()->medPr() < MAX_MED_INST ||
                (*m_options)[i].priorityInstrBlock(Server::LOW) == NULL ) )
                executePriorityInstruction( (*m_options)[i].priorityInstrBlock( Server::MEDIUM ), (*m_options)[i].instructionCounter(), Server::MEDIUM, i );
            else if( (*m_options)[i].priorityInstrBlock( Server::LOW ) != NULL )
                executePriorityInstruction( (*m_options)[i].priorityInstrBlock( Server::LOW ), (*m_options)[i].instructionCounter(), Server::LOW, i );
            else
            {
                (*m_options)[i].instructionCounter()->resetCounters();
                exec = false;
            }
            executedInstr = executedInstr || exec;
        }
        usleep( 600000 );   // 0,6 sec
    }
    return executedInstr;
}

void Scheduler::executePriorityInstruction( InstructionsBlock* &instr, Server::InstructionCounter* counter, Server::PriorityLevel lvl, int serverNum )
{
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
        instr=instr->moveToTail();

    if( lvl == Server::LOW )                                // if I execute a low level instruction, I have to reset counters
        counter->resetCounters();
}
