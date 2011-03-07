/*
    InstructionsBlock.cpp is part of BanBot.

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

#ifndef INSTRUCTIONSBLOCK_CPP
#define INSTRUCTIONSBLOCK_CPP

#include "InstructionsBlock.h"

InstructionsBlock::InstructionsBlock()
: m_next(NULL)
, m_list(NULL)
{}


InstructionsBlock::~InstructionsBlock()
{
    if (m_list != NULL)
        m_list->removeAll();
}

void InstructionsBlock::deleteAll()
{
    if ( m_next != NULL )
        m_next->deleteAll();
    delete this;
}

void InstructionsBlock::execFirstCommand( Connection* conn, int server )
{
    if ( m_list != NULL )
    {
        m_list->exec(conn,server);
        Common* temp = m_list;
        m_list = m_list->next;
        temp->remove();
    }
}

InstructionsBlock* InstructionsBlock::setNext ( InstructionsBlock* next )
{
    InstructionsBlock* t = m_next;
    m_next = next;
    return t;
}

InstructionsBlock* InstructionsBlock::getNext ()
{
    return m_next;
}

InstructionsBlock* InstructionsBlock::moveToTail ()
{
    InstructionsBlock* newHead;
    if ( m_next != NULL )
    {
         newHead = m_next;
         m_next = 0;
         newHead->addToTail( this );
    }
    else
    {
        newHead = this;
    }
    return newHead;
}

void InstructionsBlock::addToTail ( InstructionsBlock* block )
{
    if ( m_next != NULL )
        m_next->addToTail( block );
    else
        m_next = block;
}

bool InstructionsBlock::isEmpty ()
{
    if ( m_list != NULL ) return false;
    return true;
}

/* ************ command methods *********** */
//every one of this methods adds a specific instruction block at the end of the list.
//If the list is empty, it will be added as first element of the list.
void InstructionsBlock::kick( std::string number )
{
    if ( m_list !=NULL )
        m_list->addToTail( new Kick(number) );
    else
        m_list = new Kick(number);
}

void InstructionsBlock::say( std::string phrase )
{
    std::vector<std::string> rows = handyFunctions::detectRows (phrase);
    for (unsigned int i = 0; i < rows.size(); i++)
    {
        if ( m_list != NULL )
            m_list->addToTail( new Say( rows[i] ) );
        else
            m_list = new Say( rows[i] );
    }
}

void InstructionsBlock::bigtext( std::string phrase )
{
    if ( m_list != NULL )
        m_list->addToTail( new BigText(phrase) );
    else
        m_list = new BigText(phrase);
}

void InstructionsBlock::tell( std::string phrase, std::string player )
{
    std::vector<std::string> rows = handyFunctions::detectRows (phrase);
    for (unsigned int i = 0; i < rows.size(); i++)
    {
        if ( m_list != NULL )
            m_list->addToTail( new Tell( rows[i], player ) );
        else
            m_list = new Tell( rows[i], player );
    }
}

void InstructionsBlock::reload( )
{
    if ( m_list != NULL )
        m_list->addToTail( new Reload() );
    else
        m_list = new Reload();
}

void InstructionsBlock::mute( std::string number )
{
    if ( m_list != NULL )
        m_list->addToTail( new Mute(number) );
    else
        m_list = new Mute(number);
}

void InstructionsBlock::muteAll( std::string admin )
{
    if ( m_list != NULL )
        m_list->addToTail( new MuteAll(admin) );
    else
        m_list = new MuteAll(admin);
}

void InstructionsBlock::veto()
{
    if ( m_list != NULL )
        m_list->addToTail( new Veto() );
    else
        m_list = new Veto();
}

void InstructionsBlock::slap( std::string number )
{
    if ( m_list != NULL )
        m_list->addToTail( new Slap(number) );
    else
        m_list = new Slap(number);
}

void InstructionsBlock::nuke( std::string number )
{
    if ( m_list != NULL )
        m_list->addToTail( new Nuke(number) );
    else
        m_list = new Nuke(number);
}

void InstructionsBlock::force( std::string number, std::string where )
{
    if ( m_list != NULL )
        m_list->addToTail( new Force(number, where) );
    else
        m_list = new Force(number, where);
}

void InstructionsBlock::map( std::string name )
{
    if ( m_list != NULL )
        m_list->addToTail( new Map(name) );
    else
        m_list = new Map(name);
}
void InstructionsBlock::nextmap( std::string name )
{
    if ( m_list != NULL )
        m_list->addToTail( new NextMap(name) );
    else
        m_list = new NextMap(name);
}

void InstructionsBlock::changePassword( std::string pass )
{
    if ( m_list != NULL )
        m_list->addToTail( new ChangePassword(pass) );
    else
        m_list = new ChangePassword(pass);
}
void InstructionsBlock::exec( std::string file )
{
    if ( m_list != NULL )
        m_list->addToTail( new Exec(file) );
    else
        m_list = new Exec(file);
}

void InstructionsBlock::restart()
{
    if ( m_list != NULL)
        m_list->addToTail( new Restart() );
    else
        m_list = new Restart();
}

void InstructionsBlock::teamBalance()
{
    if ( m_list != NULL)
        m_list->addToTail( new TeamBalance() );
    else
        m_list = new TeamBalance();
}

void InstructionsBlock::gravity( std::string amount )
{
    if ( m_list != NULL )
        m_list->addToTail( new Gravity(amount) );
    else
        m_list = new Gravity(amount);
}

#endif // INSTRUCTIONSBLOCK_CPP