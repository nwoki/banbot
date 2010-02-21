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

//costruttore
Analyzer::Analyzer( Connection* conn, Db* db, std::string file )
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

  cout<<"[OK] Analyzer inizializzato.\n";
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
  cout<<"[OK] Analyzer avviato.\n";
  while (true)
  {
    //provo ad aprire il file e a riprendere dalla riga dove ero arrivato
    log.open(file.c_str());
    log.seekg(row);

    //se il file è aperto posso lavorare
    if (log.is_open())
    {
        cout<<"[OK] logfile founded\n";
      //il file è aperto, esamino le nuove righe (se ce ne sono)
      while (!log.eof())
      {
	//leggo una riga
	char line [1500];
	log.getline(line,1500,'\n');
	//se non è la fine del file, mi salvo la riga dove sono arrivato
	if (!log.eof()) row=log.tellg();

	cout<<line<<"\n";

	//comincio coi test
	if (isA(line, CLIENT_USER_INFO))
	{
	  cout<<"[-] Client user info\n";
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

	  cout<<"[*]Estrapolati i dati: numero="<<numero<<" guid="<<guid<<"\n";
	  //il giocatore c'è per forza (nel gioco deve fare un ClientConnect prima di userinfo)
	  //cerco il giocatore giusto all'interno della mia lista, e salvo il guid nelle info
	  unsigned int i=0;
	  bool nonTrovato=true;
	  while (nonTrovato && i<giocatori.size())
	  {
	    if (giocatori[i]->number.compare(numero)==0)
	    {
	      nonTrovato=false;
	      if (!giocatori[i]->GUID.empty() && giocatori[i]->GUID.compare(guid)!=0)
	      {
		//cambio illegale del GUID => cheats
		cout<<"[!] kick automatico per cheats.\n";
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

	  cout<<"[-] Inizio i controlli.\n";
	  //se ho il guid del giocatore procedo coi controlli
	  if (!nonTrovato)
	  {
	    //faccio un pò di controlli:
	    //controllo che non sia stato già bannato
	    cout<<"[-] CheckBanGUID()\n";
	    if(database->checkBanGuid(guid))
	    {
	      //è stato bannato, lo butto fuori
	      cout<<"[!] kick per ban.\n";
	      server->kick(numero);
	    }
	    else
	    {
	      cout<<"[-]Checking for invalid GUID...\n";
	      //ok, non è stato bannato (per il momento). Controllo se ha un GUID valido.
	      if (!isA(line, GUID))
	      {
		  //il guid è illegale, ban diretto
		  cout<<"[!] kick automatico per GUID illegale\n";
		  database->ban(guid);
		  server->kick(numero);
	      }
	    }
	  }

	  //ho finito le azioni in caso di clientUserinfo
	  cout<<"[OK] all done.";
	}
	else
	{
	  //non ha passato il test, non è un clientUserinfo: provo con gli altri regex
	  //controllo se è la connessione di un utente
	  if (isA(line, CLIENT_CONNECT))
	  {
	    cout<<"[-] Nuovo client connesso\n";
	    //è un clientconnect:
	    //prendo il numero del giocatore e mi creo il nuovo player corrispondente in memoria
	    std::string temp=line;
	    int pos=temp.find("ClientConnect:");
	    pos=temp.find_first_not_of(" ",pos+14);
	    int end=temp.find_first_of(" \n\0",pos);
	    std::string numero=temp.substr(pos,end-pos);

	    cout<<"[-] Estrapolati i dati. numero="<<numero<<"\n";
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
	      cout<<"[*] Inserisco il nuovo player\n";
	      Player * gioc=new Player();
	      gioc->number=numero;
	      giocatori.push_back(gioc);
	    }
	    else
	    {
	      cout<<"[-] Player già presente, azzerata la GUID.\n";
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
		  }
		  else i++;
		}

		//controllo se ho trovato il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
		if (!nonTrovato && database->checkAuthGuid(guid))
		{
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
		    }
		    else i++;
		  }
		  //se ho il guid, banno il player (le operazioni non sono eseguite "in diretta",
		  //per pignoleria controllo anche se per una rarissima combinazione non è già bannato).
		  if (!nonTrovato && !database->checkBanGuid(guid))
		  {
		    cout<<"[!]banning "<<guid<<"\n";
		    database->ban(guid);
		    server->kick(guid);
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
    sleep(TIME_SLEEPING);
  }
}
#endif
