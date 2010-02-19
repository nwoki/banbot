/*              logger.
 *
 *  Author:     [2s2h]n3m3s1s
 *
 *  Description:    classe che gestisce il logfile del bot. Lo si inizializza
 *                  passandogli il percorso al file log. Se il file esiste usa
 *                  quello specificato, altrimenti se lo crea.
*/

#include "logger.h"

logger::logger( string p ): path( p )
{
  try{    //check per il file log
        cout<<"[-] checking for logfile..\n";
        ifstream IN( "logs/bot.log" );
        if( !IN ) throw( 0 );
        else IN.close();
    }
    catch( int x ){
        cout<<"[!] Logfile doesn't exist..\n";
        //create logfile

    }

}

logger::~logger()
{

}

bool logger::open()
{

}

bool logger::write( string text )
{

}

void logger::close()
{

}

