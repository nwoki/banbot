/*
    InstructionsBlock.h is part of BanBot.

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
        void say( std::string phrase );    //phrase to print publically.
        void bigtext( std::string phrase );    //phrase to print publically (big).
        void tell( std::string phrase, std::string player ); //@phrase is the private message to send to @player.
        void reload();            //does a reload of the current map. If @server is -1, it does a reload on all servers.
        void mute( std::string number ); //mute/unmute the player @number.
        void muteAll( std::string admin ); //mute/unmute all players except @admin (number).
        void veto();                  //does a veto.
        void slap( std::string number ); //slaps the player @number.
        void nuke( std::string number ); //nukes the player @number.
        void force( std::string number, std::string where ); //move the player @number on the team @where
        void map( std::string name ); //change the map
        void nextmap( std::string name ); //change the nextmap
        void changePassword( std::string pass ); //change the private password of the server
        void exec( std::string file ); //load a config file for the game server

        /// TODO ask zamy : "does this method automatically move the instruction block to the end? does it delete it?
        void execFirstCommand( Connection* conn, int server );  //execute the first command of the stack on a server using the given connection class.

        /// TODO zamy: ci serve che torni un puntatore?? sinceramente lo farei "void" questo metodo
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
                void addToTail( Common* command )                       //add a command at the end of the list
                {
                    if ( next != 0 )
                        next->addToTail( command );
                    else
                        next = command;
                };
                //virtual methods
                virtual void exec ( Connection* conn, int server );     //execute this command node
                virtual void remove()                                   //delete this command node
                {
                    delete this;
                };
                virtual void removeAll()                                //delete all command nodes
                {
                    if ( next != 0 )  next->removeAll();
                    remove();
                };
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
        
        //say command node
        class Say : public Common
        {
            public:
                Say( std::string phrase ):Common(),n(phrase){};
                void exec ( Connection* conn, int server )
                {
                    conn->say( n, server );
                };
            private:
                std::string n;
        };
        
        //bigtext command node
        class BigText : public Common
        {
            public:
                BigText( std::string phrase ):Common(),n(phrase){};
                void exec ( Connection* conn, int server )
                {
                    conn->bigtext( n, server );
                };
            private:
                std::string n;
        };
        
        //tell command node
        class Tell : public Common
        {
            public:
                Tell( std::string phrase, std::string player ):Common(),n(phrase),n1(player){};
                void exec ( Connection* conn, int server )
                {
                    conn->tell( n, n1, server );
                };
            private:
                std::string n;
                std::string n1;
        };
        
        //reload command node
        class Reload : public Common
        {
            public:
                Reload():Common(){};
                void exec ( Connection* conn, int server )
                {
                    conn->reload( server );
                };
        };
        
        //mute command node
        class Mute : public Common
        {
            public:
                Mute( std::string number ):Common(),n(number){};
                void exec ( Connection* conn, int server )
                {
                    conn->mute( n, server );
                };
            private:
                std::string n;
        };
        
        //muteAll command node
        class MuteAll : public Common
        {
            public:
                MuteAll( std::string admin ):Common(),n(admin){};
                void exec ( Connection* conn, int server )
                {
                    conn->muteAll( n, server );
                };
            private:
                std::string n;
        };
        
        //veto command node
        class Veto : public Common
        {
            public:
                Veto():Common(){};
                void exec ( Connection* conn, int server )
                {
                    conn->veto( server );
                };
        };
        
        //slap command node
        class Slap : public Common
        {
            public:
                Slap( std::string number ):Common(),n(number){};
                void exec ( Connection* conn, int server )
                {
                    conn->slap( n, server );
                };
            private:
                std::string n;
        };
        
        //nuke command node
        class Nuke : public Common
        {
            public:
                Nuke( std::string number ):Common(),n(number){};
                void exec ( Connection* conn, int server )
                {
                    conn->nuke( n, server );
                };
            private:
                std::string n;
        };
        
        //force command node
        class Force : public Common
        {
            public:
                Force( std::string number, std::string where ):Common(),n(number),n1(where){};
                void exec ( Connection* conn, int server )
                {
                    conn->force( n, n1, server );
                };
            private:
                std::string n;
                std::string n1;
        };
        
        //map command node
        class Map : public Common
        {
            public:
                Map( std::string name ):Common(),n(name){};
                void exec ( Connection* conn, int server )
                {
                    conn->map( n, server );
                };
            private:
                std::string n;
        };
        
        //nextmap command node
        class NextMap : public Common
        {
            public:
                NextMap( std::string name ):Common(),n(name){};
                void exec ( Connection* conn, int server )
                {
                    conn->nextmap( n, server );
                };
            private:
                std::string n;
        };
        
        //changePassword command node
        class ChangePassword : public Common
        {
            public:
                ChangePassword( std::string pass ):Common(),n(pass){};
                void exec ( Connection* conn, int server )
                {
                    conn->changePassword( n, server );
                };
            private:
                std::string n;
        };
        
        //exec command node
        class Exec : public Common
        {
            public:
                Exec( std::string file ):Common(),n(file){};
                void exec ( Connection* conn, int server )
                {
                    conn->exec( n, server );
                };
            private:
                std::string n;
        };
        
        //pointers
        InstructionsBlock* m_next;
        Common* m_list;
};

#endif // INSTRUCTIONSBLOCK_H