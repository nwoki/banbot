/*
    server.h is part of BanBot.

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

#ifndef _server_h_
#define _server_h_

#include <sys/stat.h>
#include <string>
#include <fstream>
#include <vector>

class InstructionsBlock;

class Server
{
    public:
        Server();
        ~Server();

        enum Warnings {              //server messages (warnings) type
            DISABLED,
            PRIVATE,
            PUBLIC
        };

        enum PriorityLevel {        // server intructions priority enum
            LOW,
            MEDIUM,
            HIGH
        };

        enum Commands {             // bot instructions enum (used for permissions in servers)
            HELP,
            KICK,
            BAN,
            UNBAN,
            MUTE,
            OP,
            DEOP,
            STRICT,
            NUKE,
            SLAP,
            VETO,
            FORCE,
            MAP,
            NEXTMAP,
            ADMINS,
            PASS,
            CONFIG,
            STATUS,
            WARNINGS,
            BANTIMEWARN,
            RESTART,
            RELOAD,
            TEAMBALANCE,
            GRAVITY,
            CHANGELEVEL,
            BIGTEXT,
            TEAMS
        };

        enum Timing {               //used to define the timing of nick and ip bans.
            NEVER,
            FIVE,
            TEN,
            FIFTEEN,
            TIRTEEN,
            ALWAYS
        };

        class Player
        {
            public:
                /** which team the player is on */
                enum Team {
                    RED,
                    BLUE,
                    SPECT
                };
                
                std::string GUID
                , number
                , nick
                , ip;
                Team team;

                Player()
                    : GUID( std::string() )
                    , number( std::string() )
                    , nick( std::string() )
                    , ip( std::string() )
                    , team( SPECT )
                    {};

                /**
                 * constructor with values
                 * @param g guid
                 * @param nu player number
                 * @param ni player nick
                 * @param i player ip
                 * @param t player team
                 */
                Player( const std::string &g, const std::string &nu
                      , const std::string &ni, const std::string &i
                      , Team t )
                    : GUID(g)
                    , number(nu)
                    , nick(ni)
                    , ip(i)
                    , team(t)
                    {};

                Player* clone() { return new Player( GUID, number, nick, ip, team ); };
        };

        class InstructionCounter
        {
        public:
            InstructionCounter()
            : m_highPr( 0 )
            , m_medPr( 0 )
            , m_lowPr( 0 )
            {}

            void resetCounters()
            {
                m_highPr = 0;
                m_medPr = 0;
                m_lowPr = 0;
            }
            // increments
            void incrementPriority( Server::PriorityLevel lvl ) // increments given counter priority
            {
                if( lvl == HIGH )
                    m_highPr++;
                else if( lvl == MEDIUM )
                    m_medPr++;
                else
                    m_lowPr++;
            }

            // getters
            int highPr() const { return m_highPr; }     // returns number of high priority instruction blocks commands executed
            int medPr() const { return m_medPr; }       // returns number of medium priority instruction blocks commands executed
            int lowPr() const { return m_lowPr; }       // returns number of low priority instruction blocks commands executed

        private:
            int m_highPr, m_medPr, m_lowPr;             // counter for various priority instructions
        };

        // getters
        std::string name() const;                                           // returns server name
        std::string backupDir() const;                                      // returns path to server backup directory
        std::string botLog() const;                                         // returns path to server bot log file
        std::string configFile() const;                                     // returns path to secondary bot config file
        std::string dbFolder() const;                                       // returns path to server database folder
        struct stat infos() const;                                          // returns server infos struct
        std::string ip() const;                                             // returns server ip
        int port() const;                                                   // returns server port
        std::string rcon() const;                                           // returns server rcon pass
        std::streampos row() const;                                         // returns current row on log file
        std::string serverLog() const;                                      // returns server log path
        int strict() const;                                                 // returns strict level
        bool isChanged() const;                                             // checks if server options have been changed
        bool isValid() const;                                               // checks if server is valid
        InstructionsBlock* &priorityInstrBlock( Server::PriorityLevel lvl ); // returns pointer to InstructionBlock according to level specified
        InstructionCounter *instructionCounter() const;                     // returns pointer to server's InstructionCounter
        std::vector<std::string> serverConfigs() const;                     // returns game server's config file list
        std::string config(int index) const;                                // returns game server's config with the right index, empty string if error.
        std::vector<std::string> serverMaps() const;                        // returns game server's map file list
        std::string map(int index) const;                                   // returns game server's map with the right index, empty string if error.
        std::string gameDirectory() const;                                  // returns the directory where the game is installed
        int commandPermission ( Commands cmd ) const;                       // returns the minimum power level requested by a command.
        Warnings warnings () const;                                         // return the message type setted.
        Timing banNick () const;                                            // return the nick ban timing setted.
        Timing banIp () const;                                              // like banNick
        bool banWarnings () const;                                          // return ban warnings option (see m_banWarnings).
        std::string nextSpamMessage();                                      // return the next message.

        // setters
        void setName( std::string name );                                   // set server name
        void setBackupDir( std::string backupDir );                         // set server backup directory
        void setBotLog( std::string botLog );                               // set server botlog file
        void setConfigFile( std::string configFile );                       // set server ConfigFile
        void setDbFolder( std::string dbFolder );                           // set server database folder
        void setInfos( struct stat infos );                                 // set server infos struct
        void setIP( std::string ip );                                       // set server ip
        void setPort( int port );                                           // set server port
        void setRcon( std::string rcon );                                   // set server rcon password
        void setRow( std::streampos row );                                  // set server log row
        void setServerLog( std::string serverLog );                         // set server server log file
        void setStrict( int level = 1 );                                    // set server strict level
        void setChanged( bool changed = true );                             // set server changed flag
        void setValid( bool valid = true );                                 // set server validity flag
        void setServerConfigs( std::vector<std::string> list );             // set game server's config file list
        void setServerMaps( std::vector<std::string> list );                // set game server's extra maps list
        void setGameDirectory( std::string &dir );                          // set the directory where the game is installed
        void setCommandPermission( Commands cmd, int value );               // set the minimum power level requested by a command.
        void setWarnings ( Server::Warnings type );                         // set the warnings type (public, private or disabled)
        void setBanNick( Server::Timing time );                             // set the nick ban timing.
        void setBanIp( Server::Timing time );                               // set the ip ban timing.
        void setBanWarnings( bool option );                                 // enable or disable ban warnings (see m_banWarnings).
        void addSpamMessage( std::string );                                 // add a message to the list.


        // returns true and updates m_lastBalance
        bool permitBalance();                                               // updates m_lastBalance, and returns true if the teambalance is permitted
        bool permitTeams();                                                 // updates m_lasTeams and returns true if the teams is permitted

        // put these into scheduler and add server number to specify server to operate on
        void addPriorityInstrBlock( Server::PriorityLevel lvl, InstructionsBlock *inst );   // ADDS TO TAIL given InstructionsBlock
        void setPriorityInstrBlock( Server::PriorityLevel lvl, InstructionsBlock *inst );   // SUBSTITUTES current InstructionsBlock with the given one

        // functions that access directly the players array
        unsigned int size();                // returns the size of the player container
        Player* operator[] ( int pos );     // overloading for the operator [int]
        void push_back( Player* player );   // adds player to container
        std::vector<Server::Player*>::iterator begin();
        void erase( std::vector<Server::Player*>::iterator iteratore ); // erases given player from container
        void clear();                       // clears players container
        std::vector<std::string> getSpectNumbers(); //returns slot numbers of spects
        
        //confronto con le vecchie opzioni, ed in caso di modifiche critiche lo resetta, altrimenti ripristina i dati dinamici.
        void test_for_changes(Server* old);
        //controlla se sono state date impostate tutte le opzioni, true=ok.
        bool test_for_options();


        std::string toString();

  private:
        bool m_changed;                             // flag that indicates when a server options have been modified
        bool m_valid;                               // flag that indicates wether the server is valid or not
        std::string m_name;                         // server name
        std::string m_configFile;                   // bot config file( relative to the single server with rcon and file )
        struct stat m_infos;                        // contains config file info, as last modify time.
        std::string m_rconpass;                     // server rcon password
        std::string m_ip;                           // server ip
        int m_port;                                 // server port
        std::string m_backup;                       // server backup directory
        std::string m_botLog;                       // server botlog directory
        std::string m_serverLog;                    // log of the server to parse
        std::string m_dbFolder;                     // server database folder directory
        std::streampos m_row;                       // current row on log file
        int m_strict;                               // restriction level
        Warnings m_warnings;                        // message type (warnings)
        Timing m_banNick;                           // timing of nick ban.
        Timing m_banIp;                             // timing of ip ban.
        bool m_banWarnings;                         // define if BanBot has to advice in case of ip or nick ban over the timing limit.
        std::string m_lastBalance;                  // keeps track of last balance done.
        std::string m_lastTeams;                    // keeps track of last teams done.

        std::vector<Player*> m_giocatori;           // vector with the info of the players currently in the server
        InstructionCounter *m_instructionCounter;   // instruction counter for server's various InstructionsBlock
        InstructionsBlock *m_lowPriorityInst, *m_mediumPriorityInst, *m_highPriorityInst;   // instruction priorities

        std::vector<std::string> m_configs;         // list of cfg files founded on this server
        std::vector<std::string> m_maps;            // list of maps founded on this server
        std::string m_gameDir;                      // directory where the game in installed
        std::vector<int> m_permissions;             // minimum power level requested for each command.
        
        std::vector<std::string> m_messages;        //broadcast messages
        unsigned int m_nextMessage;                 //index of the next message to send.
};

#endif
