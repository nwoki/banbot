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

    Copyright © 2010, Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/

#ifndef INSTRUCTIONSBLOCK_CPP
#define INSTRUCTIONSBLOCK_CPP
#include "InstructionsBlock.h"

InstructionsBlock::InstructionsBlock()
: next(0)
, list(0)
{}


InstructionsBlock::~InstructionsBlock()
{
    if (list != 0)
    {
        list->removeAll();
    }
}

void InstructionsBlock::execFirstCommand( Connection* conn, int server )
{
    if ( list != 0 )
    {
        list->exec(conn,server);
        Common* temp = list;
        list = list->next;
        temp->remove();
    }
}

InstructionsBlock* InstructionsBlock::setNext ( InstructionsBlock* next )
{
    InstructionsBlock* t = this->next;
    this->next = next;
    return t;
}

InstructionsBlock* InstructionsBlock::getNext ()
{
    return next;
}

InstructionsBlock* InstructionsBlock::moveToTail ()
{
    InstructionsBlock* newHead;
    if ( next != 0 )
    {
         newHead = next;
         this->next = 0;
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
    if ( next != 0 )
    {
        next->addToTail( block );
    }
    else
    {
        next = block;
    }
}

bool InstructionsBlock::isEmpty ()
{
    if ( list != 0 ) return true;
    return false;
}

/* ************ command methods *********** */
//every one of this methods adds a specific instruction block at the end of the list.
//If the list is empty, it will be added as first element of the list.
void InstructionsBlock::kick( std::string number )
{
    if ( list !=0 )
    {
        list->addToTail( new Kick(number) );
    }
    else
    {
        list = new Kick(number);
    }
}

void InstructionsBlock::say( std::string phrase )
{
    if ( list !=0 )
    {
        list->addToTail( new Say(phrase) );
    }
    else
    {
        list = new Say(phrase);
    }
}

void InstructionsBlock::bigtext( std::string phrase )
{
    if ( list !=0 )
    {
        list->addToTail( new BigText(phrase) );
    }
    else
    {
        list = new BigText(phrase);
    }
}

void InstructionsBlock::tell( std::string phrase, std::string player )
{
    if ( list !=0 )
    {
        list->addToTail( new Tell(phrase, player) );
    }
    else
    {
        list = new Tell(phrase, player);
    }
}

void InstructionsBlock::reload( )
{
    if ( list !=0 )
    {
        list->addToTail( new Reload() );
    }
    else
    {
        list = new Reload();
    }
}

void InstructionsBlock::mute( std::string number )
{
    if ( list !=0 )
    {
        list->addToTail( new Mute(number) );
    }
    else
    {
        list = new Mute(number);
    }
}

void InstructionsBlock::muteAll( std::string admin )
{
    if ( list !=0 )
    {
        list->addToTail( new MuteAll(admin) );
    }
    else
    {
        list = new MuteAll(admin);
    }
}

void InstructionsBlock::veto()
{
    if ( list !=0 )
    {
        list->addToTail( new Veto() );
    }
    else
    {
        list = new Veto();
    }
}

void InstructionsBlock::slap( std::string number )
{
    if ( list !=0 )
    {
        list->addToTail( new Slap(number) );
    }
    else
    {
        list = new Slap(number);
    }
}

void InstructionsBlock::nuke( std::string number )
{
    if ( list !=0 )
    {
        list->addToTail( new Nuke(number) );
    }
    else
    {
        list = new Nuke(number);
    }
}

void InstructionsBlock::force( std::string number, std::string where )
{
    if ( list !=0 )
    {
        list->addToTail( new Force(number, where) );
    }
    else
    {
        list = new Force(number, where);
    }
}

void InstructionsBlock::map( std::string name )
{
    if ( list !=0 )
    {
        list->addToTail( new Map(name) );
    }
    else
    {
        list = new Map(name);
    }
}
void InstructionsBlock::nextmap( std::string name )
{
    if ( list !=0 )
    {
        list->addToTail( new NextMap(name) );
    }
    else
    {
        list = new NextMap(name);
    }
}

#endif // INSTRUCTIONSBLOCK_CPP