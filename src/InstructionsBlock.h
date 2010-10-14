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

#ifndef INSTRUCTIONSBLOCK_H
#define INSTRUCTIONSBLOCK_H

#include <string>

#include "connection.h"

class InstructionsBlock
{
    public:
        InstructionsBlock();
        ~InstructionsBlock();

        void kick( std::string number );  //number of the player to kick.
        void say( std::string frase );    //phrase to print publically.
        void bigtext( std::string frase );    //phrase to print publically (big).
        void tell( std::string frase, std::string player ); //@frase is the private message to send to @player.
        void reload( int server );            //does a reload of the current map. If @server is -1, it does a reload on all servers.
        void mute( std::string number ); //mute/unmute the player @number.
        void muteAll( std::string admin ); //mute/unmute all players except @admin (number).
        void veto();                  //does a veto.
        void slap( std::string number ); //slaps the player @number.
        void nuke( std::string number ); //nukes the player @number.
        void force( std::string number, std::string where ); //move the player @number on the team @where
        void map( std::string name ); //change the map
        void nextmap( std::string name ); //change the nextmap


        void execFirstCommand( Connection* conn, int server );  //execute the first command of the stack on @server,
                                                                //using the given connection class.

        InstructionsBlock* setNext ( InstructionsBlock* next ); //changes the next InstructionsBlock, and returns the old next.
        InstructionsBlock* getNext ();                          //get the next InstructionsBlock.
        InstructionsBlock* moveToTail ();                       //move this InstructionsBlock on the tail, and returns the new head.
        void addToTail ( InstructionsBlock* block );            //add a new block on tail.
        bool isEmpty ();                                        //returns true if all instructions are done.

    private:
        // general command node
        class Common
        {
            public:
                Common():next(0){};
                Common* next;
                virtual void exec ( Connection* conn, int server );
        };

        // kick command node
        class Kick : public Common
        {
            public:
                Kick( std::string number ):Common(),n(number){};
                void exec ( Connection* conn, int server )
                {
                    conn->kick( n, server );
                };
            private:
                std::string n;
        };

        InstructionsBlock* next;
        Common* list;
};

#endif // INSTRUCTIONSBLOCK_H
