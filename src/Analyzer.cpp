/*		Implementazione di Analyzer.
 *
 *	Author: [2s2h]Zamy
 *
 *	Note:	In main_loop() sono presenti molti controlli per evitare errori (tra cui anche segmentation fault) che a prima vista
 *		potrebbero sembrare inutili, in realtà sono necessari perchè il logging del server non è molto affidabile.
 *
*/

#ifndef _Analyzer_cpp_
#define _Analyzer_cpp_

#include "Analyzer.h"
#include <unistd.h>
#include <regex.h>

#define SOCKET_PAUSE 1   //per permettere al socket di funzionare bene, inserisco una pausa tra say e kick

//costruttore
Analyzer::Analyzer( Connection* conn, Db* db, Logger* log, std::string file ):logger(log)
{
  //imposto il file di log
  this->file=file;
  //inizializzo i puntatori agli oggetti
  server=conn;
  database=db;
  //inizializzo il resto
  row=0;
  CLIENT_CONNECT=" *[0-9]+:[0-9]{2} +ClientConnect:";
  CLIENT_USER_INFO=" *[0-9]+:[0-9]{2} +ClientUserinfo:";
  CLIENT_DISCONNECT=" *[0-9]+:[0-9]{2} +ClientDisconnect:";
  SAY=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!ban [0-9]{1,2}";
  GUID="[A-F0-9]{32}";
  INITGAME=" *[0-9]+:[0-9]{2} +InitGame:";

  logger->open();
  std::cout<<"[OK] Analyzer inizializzato.\n";
  *logger<<"[OK] Analyzer inizializzato.\n";
  logger->close();
}

//distruttore
Analyzer::~Analyzer()
{
  if (log.is_open()) log.close();
}

//testa l'array di caratteri passato col regex, torna true se la condizione imposta dal regex è soddisfatta.
bool Analyzer::isA(char* line, std::string regex)
{
  regex_t r;

  if (regcomp(&r, regex.c_str(), REG_EXTENDED|REG_NOSUB) == 0)
  {
    int status=regexec(&r, line, (size_t)0, NULL, 0);
    regfree(&r);
    if (status==0)
    {
      return true;
    }
  }
  return false;
}

//funzione da sovrascrivere nelle eventuali espansioni
void Analyzer::expansion(char* line)
{
}

//main loop
void Analyzer::main_loop()
{
  logger->open();
  std::cout<<"[OK] BanBot avviato.\n\n";
  logger->close();
  while (true)
  {
    //provo ad aprire il file e a riprendere dalla riga dove ero arrivato
    log.open(file.c_str());
    log.seekg(row);

    //se il file è aperto posso lavorare
    if (log.is_open())
    {
      logger->open();
      //il file è aperto, esamino le nuove righe (se ce ne sono)
      while (!log.eof())
      {
	//leggo una riga
	char line [1500];
	log.getline(line,1500,'\n');
	//se non è la fine del file, mi salvo la riga dove sono arrivato
	if (!log.eof()) row=log.tellg();

	//comincio coi test
	if (isA(line, CLIENT_USER_INFO))
	{
	  //ha passato il regex, è una clientUserinfo
	  //prendo il numero giocatore e la guid, utilizzando le funzioni delle stringhe
	  std::string temp=line;
	  int pos=temp.find("ClientUserinfo:");
	  pos=temp.find_first_not_of(' ',pos+15);
	  int end=temp.find_first_of(' ',pos);
	  std::string numero=temp.substr(pos,end-pos);
	  pos=temp.find("cl_guid",end);
	  pos=temp.find_first_not_of("\\",pos+7);
	  end=temp.find_first_of("\\ ",pos);
	  std::string guid=temp.substr(pos,end-pos);

	  std::cout<<"[-]Estrapolati i dati: numero="<<numero<<" guid="<<guid<<"\n";
	  logger->timestamp();
	  *logger<<"[-]Estrapolati i dati: numero="<<numero<<" guid="<<guid<<"\n";
	  //il giocatore c'è per forza (nel gioco deve fare un ClientConnect prima di userinfo)
	  //cerco il giocatore giusto all'interno della mia lista, e salvo il guid nelle info
	  unsigned int i=0;
	  bool nonTrovato=true;
	  bool kicked=false;
	  while (nonTrovato && i<giocatori.size())
	  {
	    if (giocatori[i]->number.compare(numero)==0)
	    {
	      nonTrovato=false;
	      if (!giocatori[i]->GUID.empty() && giocatori[i]->GUID.compare(guid)!=0)
	      {
		kicked=true;
		//cambio illegale del GUID => cheats
		std::cout<<"  [!] kick automatico per cheats.\n";
		*logger<<"  [!] kick automatico per cheats.\n";
		std::string frase("BanBot: kicking player number ");
		frase.append(numero);
		frase.append(" for cheats.");  
		server->say(frase);
		sleep(SOCKET_PAUSE);
		server->kick(numero);
	      }
	      else
	      {
		//tutto a posto =)
		giocatori[i]->GUID=guid;
	      }
	    }
	    else i++;
	  }

	  //se ho il guid del giocatore procedo coi controlli
	  if (!nonTrovato && !kicked)
	  {
	    //faccio un pò di controlli:
	    //controllo che non sia stato già bannato
	    if(database->checkBanGuid(guid))
	    {
	      //è stato bannato, lo butto fuori
	      std::cout<<"  [+] kick per ban.\n";
	      *logger<<"  [+] kick per ban.\n";
	      std::string frase("BanBot: kicking player number ");
	      frase.append(numero);
	      frase.append(" for ban.");
	      server->say(frase);
	      sleep(SOCKET_PAUSE);
	      server->kick(numero);
	    }
	    else
	    {
	      //ok, non è stato bannato (per il momento). Controllo se ha un GUID valido.
	      if (!isA(line, GUID))
	      {
		  //il guid è illegale, ban diretto
		  std::cout<<"  [!] kick automatico per GUID illegale\n";
		  *logger<<"  [!] kick automatico per GUID illegale\n";
		  std::string frase("BanBot: kicking player number ");
		  frase.append(numero);
		  frase.append(" for invalid guid.");
		  server->say(frase);
		  database->ban(guid);
		  sleep(SOCKET_PAUSE);
		  server->kick(numero);
	      }
	      else
	      {
		  std::cout<<"  [OK] (s)he's ok.\n";
		  *logger<<"  [OK] (s)he's ok.\n";
	      }
	    }
	  }

	  //ho finito le azioni in caso di clientUserinfo
	}
	else
	{
	  //non ha passato il test, non è un clientUserinfo: provo con gli altri regex
	  //controllo se è la connessione di un utente
	  if (isA(line, CLIENT_CONNECT))
	  {
	    //è un clientconnect:
	    //prendo il numero del giocatore e mi creo il nuovo player corrispondente in memoria
	    std::string temp=line;
	    int pos=temp.find("ClientConnect:");
	    pos=temp.find_first_not_of(" ",pos+14);
	    int end=temp.find_first_of(" \n\0",pos);
	    std::string numero=temp.substr(pos,end-pos);

	    std::cout<<"[-] Nuovo client connesso: "<<numero<<"\n";
	    logger->timestamp();
	    *logger<<"[-] Nuovo client connesso: "<<numero<<"\n";
	    //per pignoleria controllo che non sia già presente
	    unsigned int i=0;
	    bool nonTrovato=true;
	    while (nonTrovato && i<giocatori.size())
	    {
	      if (giocatori[i]->number.compare(numero)==0)
		nonTrovato=false;
	      else i++;
	    }

	    //se non è presente, lo inserisco
	    if (nonTrovato)
	    {
	      Player * gioc=new Player();
	      gioc->number=numero;
	      giocatori.push_back(gioc);
	    }
	    else
	    {
	      //se c'è già, azzero il guid
	      giocatori[i]->GUID="";
	    }
	    //ho finito le azioni in caso di clientConnect
	  }
	  else
	  {
	    //non è un clientConnect, provo con gli altri regex_t
	    //controllo se è una disconnessione
	    if (isA(line, CLIENT_DISCONNECT))
	    {
	      //è un clientDisconnect
	      //prendo il numero del giocatore e lo elimino
	      std::string temp=line;
	      int pos=temp.find("ClientDisconnect:");
	      pos=temp.find_first_not_of(" ",pos+17);
	      int end=temp.find_first_of(" \n\0",pos);
	      std::string numero=temp.substr(pos,end-pos);
	      
	      std::cout<<"[-] Client disconnesso: "<<numero<<"\n";
	      logger->timestamp();
	      *logger<<"[-] Client disconnesso: "<<numero<<"\n";
	      //cerco il player e lo elimino
	      unsigned int i=0;
	      bool nonTrovato=true;
	      while(nonTrovato && i<giocatori.size())
	      {
            if (giocatori[i]->number.compare(numero)==0)
            {
                //trovato. elimino prima l'oggetto puntato
                nonTrovato=false;
                delete giocatori[i];
                //elimino l'elemento in vector: prendo l'iteratore
                std::vector<Player*>::iterator iteratore=giocatori.begin();
                //scorro fino all'elemento corretto
                for (unsigned int j=0; j<i;j++) iteratore++;
                //elimino l'elemento
                giocatori.erase(iteratore);
            }
            else i++;
	      }

	      //finite le azioni in caso di disconnect
	    }
	    else
	    {
	      //non è neanche un clientDisconnect
	      //controllo se è un comando di ban
	      if (isA(line, SAY))
	      {
		//è una richiesta di ban:
		//controllo se la persona che l'ha richiesto ha i permessi per farlo
		std::string temp=line;
		int pos=temp.find("say:");
		pos=temp.find_first_not_of(" ",pos+4);
		int end=temp.find_first_of(" ",pos);
		std::string numero=temp.substr(pos,end-pos);

		//dal numero del richiedente, mi prendo il guid
		std::string guid="";
		unsigned int i=0;
		bool nonTrovato=true;
		while(nonTrovato && i<giocatori.size())
		{
		  if(giocatori[i]->number.compare(numero)==0)
		  {
		    guid=giocatori[i]->GUID;
		    nonTrovato=false;
		  }
		  else i++;
		}
		std::cout<<"[!] Ban requested by "<<guid<<"\n";
		logger->timestamp();
		*logger<<"[!] Ban requested by "<<guid<<"\n";
		//controllo se ho trovato il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
		if (!nonTrovato && database->checkAuthGuid(guid))
		{
		  std::cout<<"  [OK] Is an admin. Applying ban.\n";
		  *logger<<"  [OK] Is an admin. Applying ban.\n";
		  //ok ha i permessi, eseguo.
		  //prendo il numero del giocatore da bannare
		  pos=temp.find("!ban",end);
		  pos=temp.find_first_not_of(" ",pos+4);
		  end=temp.find_first_of(" ",pos);
		  numero=temp.substr(pos,end-pos);

		  //mi prendo il guid dalla lista dei giocatori (qua sto bene attento, un "utonto" potrebbe aver cappellato inserendo il numero)
		  nonTrovato=true;
		  i=0;
		  while(nonTrovato && i<giocatori.size())
		  {
		    if(giocatori[i]->number.compare(numero)==0)
		    {
		      guid=giocatori[i]->GUID;
		      nonTrovato=false;
		    }
		    else i++;
		  }
		  //se ho il guid, banno il player (le operazioni non sono eseguite "in diretta",
		  //per pignoleria controllo anche se per una rarissima combinazione non è già bannato).
		  if (!nonTrovato && !database->checkBanGuid(guid))
		  {
		    std::cout<<"  [+]banning "<<guid<<"\n";
		    *logger<<"  [+]banning "<<guid<<"\n";
		    std::string frase("BanBot: banning player number ");
		    frase.append(numero);
		    frase.append(".");
		    server->say(frase);
		    database->ban(guid);
		    sleep(SOCKET_PAUSE);
		    server->kick(numero);
		  }
		}
	      }
	      else
	      {
		//non è una richiesta di ban...
		//controllo se è l'initgame
		if (isA(line, INITGAME))
		{
		  //ok, è l'inizio di una nuova partita, resetto i player:
		  //elimino gli oggetti Player:
		  for (unsigned int i=0;i<giocatori.size();i++) delete giocatori[i];
		  //resetto il vector:
		  giocatori.clear();
		}
		else
		{
		  expansion(line);
		}
	      }
	    }
	  }
	}
      }
    }
    else
      row=0;//se non riesco ad aprire il file, ricomincio dalla prima riga
    //chiudo il file e lascio passare un po' di tempo
    log.close();
    logger->close();
    sleep(TIME_SLEEPING);
  }
}
#endif
