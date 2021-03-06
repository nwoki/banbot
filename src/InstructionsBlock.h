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

    Copyright © 2010, 2011 Zamy (Simone Daminato), N3m3s1s (Francesco Nwokeka)


    BanBot uses SQLite3:
    Copyright (C) 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2004, 2005 Free
    Software Foundation, Inc.
*/

#ifndef INSTRUCTIONSBLOCK_H
#define INSTRUCTIONSBLOCK_H

#include <string>
#include <regex.h>

#include "connection.h"

class InstructionsBlock
{
    public:
        InstructionsBlock();
        ~InstructionsBlock();
        void deleteAll();           //delete the entire list of InstrutionsBlocks.

        void kick( std::string number );  //number of the player to kick.
        void say( std::string phrase );    //phrase to print publically.
        void bigtext( std::string phrase );    //phrase to print publically (big).
        void tell( std::string phrase, std::string player ); //@phrase is the private message to send to @player.
        void reload();            //does a reload of the current map.
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
        void restart(); //restart the current map.
        void teamBalance( std::vector<std::string> excludelist ); //balance teams
        void gravity( std::string amount ); //changes the gravity
        void playersInfo( std::string player ); //send info on online players to the admin @player (with a pm).
        void cycle(); //load the next map.

        void execFirstCommand( Connection* conn, int server );  //execute the first command of the stack on a server using the given connection class.

        InstructionsBlock* setNext ( InstructionsBlock* next ); //changes the next InstructionsBlock, and returns the old next.
        InstructionsBlock* getNext ();                          //get the next InstructionsBlock.
        InstructionsBlock* moveToTail ();                       //move this InstructionsBlock on the tail, and returns the new head.
        void addToTail ( InstructionsBlock* block );            //add a new block on tail.
        bool isEmpty ();                                        //returns true if all instructions are done.

    private:
        struct Info{                    //used in team balance,
        std::string number;
        int score;
        };
        
        // general command node
        class Common
        {
            public:
                Common():next(0){};
                virtual ~Common(){};
                Common* next;
                void addToTail( Common* command )                       //add a command at the end of the list
                {
                    if ( next != NULL )
                        next->addToTail( command );
                    else
                        next = command;
                };
                //virtual methods
                virtual void exec ( Connection* conn, int server ){}     //execute this command node
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
                {
                    conn->reload( server );
                };
        };
        
        //mute command node
        class Mute : public Common
        {
            public:
                Mute( std::string number ):Common(),n(number){};
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
                {
                    conn->veto( server );
                };
        };
        
        //slap command node
        class Slap : public Common
        {
            public:
                Slap( std::string number ):Common(),n(number){};
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
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
                virtual void exec ( Connection* conn, int server )
                {
                    conn->exec( n, server );
                };
            private:
                std::string n;
        };
        
        //restart command node
        class Restart : public Common
        {
            public:
                Restart():Common(){};
                virtual void exec ( Connection* conn, int server )
                {
                    conn->restart( server );
                };
        };
        
        //teamBalance command node
        class TeamBalance : public Common
        {
            public:
                TeamBalance(const std::vector<std::string> &exclude, int tryed = 0):Common(),tentativi(tryed),exclude(exclude){};
                virtual void exec ( Connection* conn, int server )
                {
                    std::string temp = conn->status( server );
                    std::vector<handyFunctions::stats> players;
                    if ( handyFunctions::extractFromStatus(temp,&players) ){
                        
                        //delete all spects, to leave them untouched,
                        for (unsigned int i=0; i<players.size(); i++){
                            bool found = false;
                            for (unsigned int j = 0; !found && j<exclude.size(); j++){
                                if ( exclude[j].compare(players[i].slot) == 0 ){
                                    found = true;
                                    #ifdef DEBUG_MODE
                                    std::cout<<"Excluding "<<players[i].nick<<"\n";
                                    #endif
                                    players.erase(players.begin()+i);
                                    exclude.erase(exclude.begin()+j);
                                    i--;
                                }
                            }
                        }

                        if (players.size() > 0){
                            
                            //order them
                            unsigned int indexes [players.size()];
                            for (unsigned int i=0; i<players.size(); i++) indexes[i] = i;
                            for (unsigned int i=0; i<players.size()-1; i++)
                            {
                                for (unsigned int j=i+1; j<players.size(); j++)
                                {
                                    if (players.at(indexes[i]).score < players.at(indexes[j]).score)
                                    {
                                        unsigned int t = indexes[i];
                                        indexes[i] = indexes[j];
                                        indexes[j] = t;
                                    }
                                }
                            }
                            
                            for (unsigned int i=0; i<players.size(); i++)
                            {
                                if (i%2 == 0)
                                {
                                    addToTail( new Force(players.at(indexes[i]).slot,"red") );
                                    #ifdef DEBUG_MODE
                                    std::cout<<players.at(indexes[i]).slot<<" to red.\n";
                                    #endif
                                }
                                else
                                {
                                    addToTail( new Force(players.at(indexes[i]).slot,"blue") );
                                    #ifdef DEBUG_MODE
                                    std::cout<<players.at(indexes[i]).slot<<" to blue.\n";
                                    #endif
                                }
                            }
                            #ifdef ITA
                            addToTail( new Say("^0BanBot: ^1 bilanciamento teams finito.") );
                            #else
                            addToTail( new Say("^0BanBot: ^1 end of team balance.") );
                            #endif
                        }
                    } 
                    else {
                        if ( tentativi < 3 ){
                            tentativi++;
                            std::string phrase;
                            #ifdef ITA
                            phrase.append("^0BanBot: ^1rcon status fallito, riprovo (^2");
                            phrase.append(handyFunctions::intToString(tentativi));
                            phrase.append("°^1 tentativo).");
                            #else
                            phrase.append("^0BanBot: ^1rcon status failed, trying again (tryed ^2");
                            phrase.append(handyFunctions::intToString(tentativi));
                            phrase.append("^1 times).");
                            #endif
                            
                            #ifdef DEBUG_MODE
                            std::cout<<"status failed, trying again.\n";
                            #endif
                            
                            addToTail( new Say(phrase) );
                            addToTail( new TeamBalance(exclude, tentativi) );
                        }
                        else{
                            std::string phrase;
                            #ifdef ITA
                            phrase.append("^0BanBot: ^1rcon status fallito: operazione annullata.");
                            #else
                            phrase.append("^0BanBot: ^1rcon status failed: command aborted.");
                            #endif
                            
                            addToTail( new Say(phrase) );
                        }
                    }
                };
            private:
                int tentativi;
                std::vector<std::string> exclude;
        };
        
        //gravity command node
        class Gravity : public Common
        {
            public:
                Gravity( std::string amount ):Common(),n(amount){};
                virtual void exec ( Connection* conn, int server )
                {
                    conn->gravity( n, server );
                };
            private:
                std::string n;
        };
        
        //pointers
        InstructionsBlock* m_next;
        Common* m_list;
        
        //players command node
        class PlayersInfo : public Common
        {
            public:
                PlayersInfo(std::string player, int tryed = 0):Common(),tentativi(tryed),number(player){};
                virtual void exec ( Connection* conn, int server )
                {
                    std::string temp = conn->status( server );
                    std::vector<handyFunctions::stats> players;
                    if ( handyFunctions::extractFromStatus(temp,&players) ){
                        std::string phrase;
                        #ifdef ITA
                        phrase.append("^0BanBot: ^1 informazioni sui player :");
                        #else
                        phrase.append("^0BanBot: ^1player's info :");
                        #endif
                        addToTail( new Tell(phrase,number) );
                        phrase.clear();
                        for (unsigned int i = 0; i < players.size(); i++){
                            phrase.append("^2");
                            phrase.append(players.at(i).slot);
                            phrase.append(" ^1");
                            phrase.append(players.at(i).nick);
                            phrase.append(" ^2");
                            phrase.append(players.at(i).ip);
                            phrase.append(" ^1");
                            phrase.append(players.at(i).port);
                            addToTail( new Tell(phrase,number) );
                            phrase.clear();
                        }
                    } 
                    else {
                        if ( tentativi < 3 ){
                            tentativi++;
                            std::string phrase;
                            #ifdef ITA
                            phrase.append("^0BanBot: ^1rcon status fallito, riprovo (^2");
                            phrase.append(handyFunctions::intToString(tentativi));
                            phrase.append("°^1 tentativo).");
                            #else
                            phrase.append("^0BanBot: ^1rcon status failed, trying again (tryed ^2");
                            phrase.append(handyFunctions::intToString(tentativi));
                            phrase.append("^1 times).");
                            #endif
                            
                            #ifdef DEBUG_MODE
                            std::cout<<"status failed, trying again.\n";
                            #endif
                            
                            addToTail( new Tell(phrase,number) );
                            addToTail( new PlayersInfo( number, tentativi) );
                        }
                        else{
                            std::string phrase;
                            #ifdef ITA
                            phrase.append("^0BanBot: ^1rcon status fallito: operazione annullata.");
                            #else
                            phrase.append("^0BanBot: ^1rcon status failed: command aborted.");
                            #endif
                            
                            addToTail( new Tell(phrase,number) );
                        }
                    }
                };
            private:
                int tentativi;
                std::string number;
        };
        
        //cycle command node
        class Cycle : public Common
        {
            public:
                Cycle():Common(){};
                virtual void exec ( Connection* conn, int server )
                {
                    conn->cycle( server );
                };
        };
};

#endif // INSTRUCTIONSBLOCK_H
