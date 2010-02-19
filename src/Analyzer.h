/*				Analyzer.
 *
 *	Author: 	[2s2h]Zamy
 *
 *	Description:	Fa da interfaccia ai file di log, si preoccupa di controllare gli utenti connessi ed i ban.
 *			Utilizza un'altra classe per accedere al database (per controllare gli utenti bannati) e per
 *			accedere al server (per kickare i giocatori).
 *
 *	Note:		Questa classe è progettata per essere facilmente espandibile nelle sue funzionalità.
 *			Per fare ciò, è sufficiente scrivere una classe che eredita da Analyzer, e ne sovrascrive 
 *			il metodo virtuale "expansion(char* line)": infatti tutte le righe che non vengono "catturate"
 *			da Analyzer, vengono passate a questo metodo.
 *			Per effettuare test sulle righe per cercarne di particolari (per esempio un say che contiene
 *			la stringa "!slap") consiglio caldamente l'utilizzo del metodo isA(char* line,std::string regex),
 *			scritto apposta per eseguire questi test utilizzando i regex.
 *
*/

#ifndef _Analyzer_h_
#define _Analyzer_h_
#include <fstream>
#include <string>
#include <vector>
#include "connection.h"
#include "db.h"

class Analyzer
{
  private:
    class Player
    {
      public:
	std::string GUID;
	std::string number;
	Player():GUID(std::string("")),number(""){};
    };
    std::vector<Player*> giocatori;
    static const long TIME_SLEEPING = 10;
    std::ifstream log;
    const char* CLIENT_CONNECT;
    const char* CLIENT_USER_INFO;
    const char* CLIENT_DISCONNECT;
    const char* SAY;
    const char* GUID;
    const char* INITGAME;
    std::string file;
    std::streampos row;
    Connection * server;
    Db * database;
    
  protected:
    bool isA(char* line,std::string regex);
    virtual void expansion(char* line);
    
  public:
    Analyzer(Connection*,Db*,std::string=NULL);
    ~Analyzer();
    void main_loop();
    void setFile(std::string);
};

#endif