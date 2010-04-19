/*
    Analyzer.cpp is part of BanBot.

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

#ifndef _Analyzer_cpp_
#define _Analyzer_cpp_

#include "Analyzer.h"
#include <unistd.h>
#include <regex.h>

//#include <time.h>
//#include <stdlib.h>
//#include <sys/stat.h>

//costruttore
Analyzer::Analyzer( Connection* conn, Db* db,Logger* primaryLog, Backup* backup, std::vector<ConfigLoader::Option> opzioni ):
    backup(backup),
    generalLog( primaryLog ),
    logger( new Logger() ),
    server( conn ),
    database( db ),
    serverNumber( 0 )
{
  //imposto i file di log e i puntatori alla riga, ne sfrutto il numero per inizializzare pure l'array per i giocatori
    for (unsigned int i = 0; i < opzioni.size(); i++ ){
        if( opzioni[i].name.compare( "LOGPATH" ) == 0 ){
            files.push_back( opzioni[i].value );
            row.push_back( 0 );
            std::vector<Player*> t;
            giocatori.push_back( t );
        }
        if ( opzioni[i].name.compare( "BOTLOGPATH" ) == 0 )
            BotLogFiles.push_back( opzioni[i].value );
  }
  //inizializzo il resto
  CLIENT_CONNECT=" *[0-9]+:[0-9]{2} +ClientConnect:";
  CLIENT_USER_INFO=" *[0-9]+:[0-9]{2} +ClientUserinfo:";
  CLIENT_DISCONNECT=" *[0-9]+:[0-9]{2} +ClientDisconnect:";
  BAN=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!ban [0-9]{1,2}";
  FIND=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!find [^ \t\n\r\f\v]+";
  FINDOP=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!findop [^ \t\n\r\f\v]+";
  UNBAN=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!unban [0-9]+";
  OP=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!op [0-9]{1,2}";
  DEOP=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!deop [0-9]+";
  GUID="[A-F0-9]{32}";
  INITGAME=" *[0-9]+:[0-9]{2} +InitGame:";
  HELP=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!help";
  KICK=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!kick [0-9]{1,2}";
  MUTE=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!mute [0-9]{1,2}";
  UNMUTE=" *[0-9]+:[0-9]{2} +say: +[0-9]+ +[^ \t\n\r\f\v]+: +!unmute [0-9]{1,2}";

  server->reload();
  std::cout<<"[OK] Analyzer inizializzato.\n";
  *generalLog<<"[OK] Analyzer inizializzato.\n\n";
}

//distruttore
Analyzer::~Analyzer()
{
  if( log.is_open() ) log.close();
}

//testa l'array di caratteri passato col regex, torna true se la condizione imposta dal regex è soddisfatta.
bool Analyzer::isA( char* line, const std::string& regex )
{
    regex_t r;

    if ( regcomp( &r, regex.c_str(), REG_EXTENDED|REG_NOSUB ) == 0){
        int status = regexec( &r, line, ( size_t )0, NULL, 0 );
        regfree( &r );
        if( status == 0 )
            return true;
    }
    return false;
}

bool Analyzer::isAdminSay( char* line, std::string &numero )
{
    //controllo se la persona che l'ha richiesto ha i permessi per farlo
    std::string temp = line;
    int pos = temp.find( "say:" );
    pos = temp.find_first_not_of( " ", pos+4 );
    int end = temp.find_first_of( " ", pos );
    numero = temp.substr( pos, end-pos );

    //dal numero del richiedente, mi prendo il guid
    std::string guid( "" );
    std::string nick( "" );
    unsigned int i = 0;
    bool nonTrovato = true;

    while( nonTrovato && i < giocatori[serverNumber].size() ){
        if( giocatori[serverNumber][i]->number.compare( numero ) == 0 ){
            guid = giocatori[serverNumber][i]->GUID;
            nick = giocatori[serverNumber][i]->nick;
            nonTrovato = false;
        }
        else i++;
    }

    std::cout<<" requested by "<<nick<<", "<<guid<<"\n";
    *logger<<" requested by "<<nick<<", "<<guid<<"\n";
    if( !nonTrovato && database->checkAuthGuid(correggi(guid)))
        return true;

    return false;
}

//funzione da sovrascrivere nelle eventuali espansioni
void Analyzer::expansion(char* line)
{
}

void Analyzer::clientUserInfo(char* line)
{
  //prendo il numero giocatore e la guid, utilizzando le funzioni delle stringhe
  std::string temp=line;
  int pos=temp.find("ClientUserinfo:");
  pos=temp.find_first_not_of(' ',pos+15);
  int end=temp.find_first_of(' ',pos);
  std::string numero=temp.substr(pos,end-pos);
  pos=temp.find("ip",end);
  pos=temp.find_first_not_of("\\",pos+3);
  end=temp.find_first_of("\\ ",pos);
  std::string ip=temp.substr(pos,end-pos);
  pos=temp.find("name",end);
  pos=temp.find_first_not_of("\\",pos+4);
  end=temp.find_first_of("\\ ",pos);
  std::string nick=temp.substr(pos,end-pos);
  pos=temp.find("cl_guid",end);
  pos=temp.find_first_not_of("\\",pos+7);
  end=temp.find_first_of("\\ ",pos);
  std::string guid=temp.substr(pos,end-pos);

  std::cout<<"[-]Estrapolati i dati: numero="<<numero<<" guid="<<guid<<" nick="<<nick<<" ip="<<ip<<"\n";
  logger->timestamp();
  *logger<<"\n[-]Estrapolati i dati: numero="<<numero<<" guid="<<guid<<" nick="<<nick<<" ip="<<ip<<"\n";
  //il giocatore c'è per forza (nel gioco deve fare un ClientConnect prima di userinfo)
  //cerco il giocatore giusto all'interno della mia lista, e salvo il guid nelle info
  unsigned int i=0;
  bool nonTrovato=true;
  bool kicked=false;
  while (nonTrovato && i<giocatori[serverNumber].size())
  {
    if (giocatori[serverNumber][i]->number.compare(numero)==0)
    {
      nonTrovato=false;
      if (!giocatori[serverNumber][i]->GUID.empty() && giocatori[serverNumber][i]->GUID.compare(guid)!=0)
      {
        kicked=true;
        //cambio illegale del GUID => cheats
        //è inutile inserirlo nel db, tanto cambia continuamente guid
        std::cout<<"  [!] kick automatico per cheats.\n";
        *logger<<"  [!] kick automatico per cheats.\n";
        std::string frase("BanBot: kicking player number ");
        frase.append(numero);
        frase.append(" for cheats.");
        server->say(frase,serverNumber);
        std::string ora;
        std::string data;
        std::string motivo("automated ban 4 cheats (guid changed during the game).");
        getDateAndTime(data,ora);
        database->ban(correggi(nick),ip,data,ora,correggi(guid),correggi(motivo));
        sleep(SOCKET_PAUSE);
        server->kick(numero,serverNumber);
      }
      else
      {
        //tutto a posto =)
        //salvo guid e nick, inserisco il nick e l'ip nel db
        giocatori[serverNumber][i]->nick=nick;
        giocatori[serverNumber][i]->GUID=guid;
        giocatori[serverNumber][i]->ip=ip;
      }
    }
    else i++;
  }

  //se ho il guid del giocatore procedo coi controlli
  if (!nonTrovato && !kicked)
  {
    //faccio un pò di controlli:
    //controllo che non sia stato già bannato
    //controllo se la guid è stata bannata
    if(database->checkBanGuid(guid))
    {
      std::string query("SELECT banned.motive,banned.id FROM banned join guids ON banned.id=guids.banId WHERE guids.guid='");
      query.append(correggi(guid));
      query.append("';");
      std::vector<std::string> risultato=database->extractData(query);

      //butto fuori la persona dal server
      buttaFuori(risultato,numero,nick);

      //aggiorno i dati sul database
      if (risultato.size()>1)
      {
        std::string ora;
        std::string data;
        getDateAndTime(data,ora);
        database->modifyBanned(correggi(nick),ip,data,ora,std::string(),risultato[1]);
      }
    }
    //controllo se il nick è bannato
    else if (nickIsBanned(nick))
      {
        std::string query("SELECT motive,id FROM banned WHERE nick='");
        query.append(correggi(guid));
        query.append("';");
        std::vector<std::string> risultato=database->extractData(query);

        buttaFuori(risultato, numero,nick);

        if (risultato.size()>1)
        {
          std::string ora;
          std::string data;
          getDateAndTime(data,ora);
          database->modifyBanned(std::string(),ip,data,ora,std::string(),risultato[1]);
          database->insertNewGuid(correggi(guid),risultato[1]);
        }
      }
      else if (ipIsBanned(ip))
        {
          std::string query("SELECT motive,id FROM banned WHERE ip='");
          query.append(ip);
          query.append("';");
          std::vector<std::string> risultato=database->extractData(query);

          buttaFuori(risultato, numero,nick);

          if (risultato.size()>1)
          {
            std::string ora;
            std::string data;
            getDateAndTime(data,ora);
            database->modifyBanned(correggi(nick),std::string(),data,ora,std::string(),risultato[1]);
            database->insertNewGuid(correggi(guid),risultato[1]);
          }
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
              frase.append(", ");
              frase.append(nick);
              frase.append(" for invalid guid.");
              server->say(frase,serverNumber);
              std::string ora;
              std::string data;
              std::string motivo("automated ban 4 cheats (invalid GUID).");
              getDateAndTime(data,ora);
              database->ban(correggi(nick),ip,data,ora,correggi(guid),correggi(motivo));
              sleep(SOCKET_PAUSE);
              server->kick(numero,serverNumber);
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

void Analyzer::clientConnect(char* line)
{
  //prendo il numero del giocatore e mi creo il nuovo player corrispondente in memoria
  std::string temp=line;
  int pos=temp.find("ClientConnect:");
  pos=temp.find_first_not_of(" ",pos+14);
  int end=temp.find_first_of(" \n\0",pos);
  std::string numero=temp.substr(pos,end-pos);

  std::cout<<"[-] Nuovo client connesso: "<<numero<<"\n";
  logger->timestamp();
  *logger<<"\n[-] Nuovo client connesso: "<<numero<<"\n";
  //per pignoleria controllo che non sia già presente
  unsigned int i=0;
  bool nonTrovato=true;
  while (nonTrovato && i<giocatori[serverNumber].size())
  {
    if (giocatori[serverNumber][i]->number.compare(numero)==0)
      nonTrovato=false;
    else i++;
  }

  //se non è presente, lo inserisco
  if (nonTrovato)
  {
    Player * gioc=new Player();
    gioc->number=numero;
    giocatori[serverNumber].push_back(gioc);
  }
  else
  {
    //se c'è già, azzero il guid
    giocatori[serverNumber][i]->GUID="";
  }
  //ho finito le azioni in caso di clientConnect
}

void Analyzer::clientDisconnect(char* line)
{
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
  while(nonTrovato && i<giocatori[serverNumber].size())
  {
    if (giocatori[serverNumber][i]->number.compare(numero)==0)
    {
    //trovato. elimino prima l'oggetto puntato
    nonTrovato=false;
    delete giocatori[serverNumber][i];
    //elimino l'elemento in vector: prendo l'iteratore
    std::vector<Player*>::iterator iteratore=giocatori[serverNumber].begin();
    //scorro fino all'elemento corretto
    for (unsigned int j=0; j<i;j++) iteratore++;
    //elimino l'elemento
    giocatori[serverNumber].erase(iteratore);
    }
    else i++;
  }
  //finite le azioni in caso di disconnect
}

void Analyzer::ban(char* line)
{
  std::cout<<"[!] Ban";
  logger->timestamp();
  *logger<<"\n[!] Ban";
  //controllo se ho trovato il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numeroAdmin;
  if (isAdminSay(line,numeroAdmin))
  {
    std::cout<<"  [OK] Is an admin. Applying ban.\n";
    *logger<<"  [OK] Is an admin. Applying ban.\n";
    //ok ha i permessi, eseguo.
    //prendo il numero del giocatore da bannare
    std::string temp(line);
    unsigned int pos=temp.find("!ban");
    pos=temp.find_first_of("0123456789",pos+4);
    int end=temp.find_first_of(" ",pos);
    std::string numero=temp.substr(pos,end-pos);
    pos=temp.find_first_not_of(" ",end);
    std::string motivo;
    if (pos<temp.size()) motivo=temp.substr(pos,temp.size()-pos);

    //mi prendo il guid e il nick dalla lista dei giocatori (qua sto bene attento, un "utonto" potrebbe aver cappellato inserendo il numero)
    bool nonTrovato=true;
    std::string nick("");
    std::string guid("");
    unsigned int i=0;
    while(nonTrovato && i<giocatori[serverNumber].size())
    {
      if(giocatori[serverNumber][i]->number.compare(numero)==0)
      {
        guid=giocatori[serverNumber][i]->GUID;
        nick=giocatori[serverNumber][i]->nick;
        nonTrovato=false;
      }
      else i++;
    }
    //se ho il guid, banno il player (le operazioni non sono eseguite "in diretta",
    //per pignoleria controllo anche se per una rarissima combinazione non è già bannato).
    if (!nonTrovato && !database->checkBanGuid(guid))
    {
      std::cout<<"  [+]banning "<<nick<<" with guid "<<guid<<"\n";
      *logger<<"  [+]banning "<<nick<<" with guid "<<guid<<"\n";
      std::string frase("BanBot: banning player number ");
      frase.append(numero);
      frase.append(", ");
      frase.append(nick);
      frase.append(" for ");
      frase.append(motivo);
      frase.append(".");
      std::cout<<"Passo 1\n";
      server->say(frase,serverNumber);
      std::string ora;
      std::string data;
      getDateAndTime(data,ora);
      database->ban(correggi(nick),giocatori[serverNumber][i]->ip,data,ora,correggi(guid),correggi(motivo));
      sleep(SOCKET_PAUSE);
      server->kick(numero,serverNumber);
    }
    else
    {
      std::cout<<"  [!]fail: player not in-game or already banned\n";
      *logger<<"  [!]fail: player not in-game or already banned\n";
      server->tell("Banning error: player not in-game or already banned",numeroAdmin,serverNumber);
    }
  }
}

void Analyzer::unban(char* line)
{
  std::cout<<"[!] Unban";
  logger->timestamp();
  *logger<<"\n[!] Unban";
  //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numeroAdmin;
  if (isAdminSay(line,numeroAdmin))
  {
    //prendo l'identificativo da sbannare
    std::string temp(line);
    int pos=temp.find("!unban");
    pos=temp.find_first_of("0123456789",pos+6);
    int end=temp.find_first_of(" ",pos);
    std::string numero=temp.substr(pos,end-pos);

    //ho il numero, elimino dal database tutti i record relativi.
    std::string query("DELETE FROM guids WHERE banId='");
    query.append(numero);
    query.append("';");
    database->extractData(query);
    std::string frase;
    if (database->deleteBanned(numero))
      frase.append("BanBot: utente sbannato con successo.");
    else frase.append("BanBot: è stato riscontrato un errore, utente non sbannato.");
    server->tell(frase,numeroAdmin,serverNumber);
  }
}

void Analyzer::find(char* line)
{
  std::cout<<"[!] Find";
  logger->timestamp();
  *logger<<"\n[!] Find";
  //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numero;
  if (isAdminSay(line,numero))
  {
    std::cout<<"  [OK] Is an admin. Doing find.\n";
    *logger<<"  [OK] Is an admin. Doing find.\n";

    //estraggo il nick da cercare.
    std::string temp(line);
    int pos=temp.find("!find");
    std::string nick=temp.substr(pos+6);

    //ho il nick da cercare
    std::cout<<"  [-]Searching for "<<nick<<".\n";
    *logger<<"  [-]Searching for "<<nick<<".\n";
    //eseguo la ricerca sul DB e invio i risultati al server di gioco.
    
    std::string query("SELECT id,nick,motive FROM banned WHERE nick = '");
    query.append(correggi(nick));
    query.append("' LIMIT 7;");
    std::vector<std::string> risultato=database->extractData(query);
    
    query.clear();
    query="SELECT id,nick,motive FROM banned WHERE nick LIKE '%";
    query.append(correggi(nick));
    query.append("%' LIMIT 16;");
    std::vector<std::string> risultatoApprossimativo=database->extractData(query);
    
    std::string frase("Risultati esatti: ");
    if (risultato.size()>18)
    {
      frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi ");
      frase.append(risultato[0]);
      frase.append(" ");
      frase.append(risultato[1]); 
      frase.append(" ");
      frase.append(risultato[2]);
      frase.append(" ?");
    }
    else
    {
      if (risultato.empty()) frase.append("none.");
      else for (unsigned int i=0; i<risultato.size();i+=3)
        {
          frase.append(risultato[i]);
          frase.append(" ");
          frase.append(risultato[i+1]);
          frase.append(" ");
          frase.append(risultato[i+2]);
          if(i<risultato.size()-3) frase.append(", ");
          else frase.append(".");
        }
    }
    tell(frase,numero);
    
    frase.clear();
    frase.append("Ricerca: ");
    if (risultatoApprossimativo.size()>45)
    {
      frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi ");
      frase.append(risultatoApprossimativo[0]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[1]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[2]);
      frase.append(", ");
      frase.append(risultatoApprossimativo[3]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[4]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[5]);
      frase.append(" ?");
    }
    else
    {
      if (risultatoApprossimativo.empty()) frase.append("none.");
      else for (unsigned int i=0; i<risultatoApprossimativo.size();i+=3)
        {
          frase.append(risultatoApprossimativo[i]);
          frase.append(" ");
          frase.append(risultatoApprossimativo[i+1]);
          frase.append(" ");
          frase.append(risultatoApprossimativo[i+2]);
          if(i<risultatoApprossimativo.size()-3) frase.append(", ");
          else frase.append(".");
        }
    }
    tell(frase,numero);
  }
}

void Analyzer::findOp(char* line)
{
  std::cout<<"[!] FindOp";
  logger->timestamp();
  *logger<<"\n[!] FindOp";
  //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numero;
  if (isAdminSay(line,numero))
  {
    std::cout<<"  [OK] Is an admin. Doing findop.\n";
    *logger<<"  [OK] Is an admin. Doing findop.\n";

    //estraggo il nick da cercare.
    std::string temp(line);
    int pos=temp.find("!findop");
    std::string nick=temp.substr(pos+8);

    //ho il nick da cercare
    std::cout<<"  [-]Searching for "<<nick<<".\n";
    *logger<<"  [-]Searching for "<<nick<<".\n";
    //eseguo la ricerca sul DB e invio i risultati al server di gioco.
    
    std::string query("SELECT id,nick FROM oplist WHERE nick = '");
    query.append(correggi(nick));
    query.append("' LIMIT 7;");
    std::vector<std::string> risultato=database->extractData(query);
    
    query.clear();
    query="SELECT id,nick FROM oplist WHERE nick LIKE '%";
    query.append(correggi(nick));
    query.append("%' LIMIT 16;");
    std::vector<std::string> risultatoApprossimativo=database->extractData(query);
    
    std::string frase("Risultati esatti: ");
    if (risultato.size()>18)
    {
      frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi ");
      frase.append(risultato[0]);
      frase.append(" ");
      frase.append(risultato[1]);
      frase.append(" ");
      frase.append(risultato[2]);
      frase.append(" ?");
    }
    else
    {
      if (risultato.empty()) frase.append("none.");
      else for (unsigned int i=0; i<risultato.size();i+=3)
        {
          frase.append(risultato[i]);
          frase.append(" ");
          frase.append(risultato[i+1]);
          frase.append(" ");
          frase.append(risultato[i+2]);
          if(i<risultato.size()-3) frase.append(", ");
          else frase.append(".");
        }
    }
    tell(frase,numero);
    
    frase.clear();
    frase.append("Ricerca: ");
    if (risultatoApprossimativo.size()>45)
    {
      frase.append("troppi risultati, prova a migliorare la ricerca. Forse cercavi ");
      frase.append(risultatoApprossimativo[0]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[1]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[2]);
      frase.append(", ");
      frase.append(risultatoApprossimativo[3]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[4]);
      frase.append(" ");
      frase.append(risultatoApprossimativo[5]);
      frase.append(" ?");
    }
    else
    {
      if (risultatoApprossimativo.empty()) frase.append("none.");
      else for (unsigned int i=0; i<risultatoApprossimativo.size();i+=3)
        {
          frase.append(risultatoApprossimativo[i]);
          frase.append(" ");
          frase.append(risultatoApprossimativo[i+1]);
          frase.append(" ");
          frase.append(risultatoApprossimativo[i+2]);
          if(i<risultatoApprossimativo.size()-3) frase.append(", ");
          else frase.append(".");
        }
    }
    tell(frase,numero);
  }
}

void Analyzer::op(char* line)
{
  std::cout<<"[!] Op";
  logger->timestamp();
  *logger<<"\n[!] Op";
  //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numeroAdmin;
  if (isAdminSay(line,numeroAdmin))
  {
    //prendo il numero del player da aggiungere tra gli admin
    std::string temp(line);
    int pos=temp.find("!op");
    pos=temp.find_first_of("0123456789",pos+3);
    int end=temp.find_first_of(" ",pos);
    std::string numero=temp.substr(pos,end-pos);
    
    //prendo i dati dell'utente e lo aggiungo tra gli op
    unsigned int i=0;
    bool nonTrovato=true;
    while (nonTrovato && i<giocatori[serverNumber].size())
    {
      if (giocatori[serverNumber][i]->number.compare(numero)==0)
        nonTrovato=false;
      else i++;
    }
    if (nonTrovato)
      server->tell("Error: player not found.",numeroAdmin,serverNumber);
    else
    {
      if(database->checkAuthGuid(giocatori[serverNumber][i]->GUID) && database->addOp(giocatori[serverNumber][i]->nick,giocatori[serverNumber][i]->GUID))
        server->tell("BanBot: admin aggiunto con successo.",numeroAdmin,serverNumber);
      else
        server->tell("Fail: player non aggiunto alla lista admin.",numeroAdmin,serverNumber);
    }
  }
}

void Analyzer::deop(char* line)
{
  std::cout<<"[!] Deop";
  logger->timestamp();
  *logger<<"\n[!] Deop";
  //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numeroAdmin;
  if (isAdminSay(line,numeroAdmin))
  {
    //prendo i dati dell'utente e lo tolgo dagli op
    std::string temp(line);
    int pos=temp.find("!deop");
    pos=temp.find_first_of("0123456789",pos+5);
    int end=temp.find_first_of(" ",pos);
    std::string numero=temp.substr(pos,end-pos);

    //ho il numero, elimino dal database tutti i record relativi.
    std::string frase;
    if (database->deleteOp(numero))
      frase.append("BanBot: utente tolto con successo dalla lista admin.");
    else frase.append("BanBot: è stato riscontrato un errore.");
    server->tell(frase,numeroAdmin,serverNumber);
  }
}

void Analyzer::kick(char* line)
{
  std::cout<<"[!] Kick";
  logger->timestamp();
  *logger<<"\n[!] Kick";
  //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numeroAdmin;
  if (isAdminSay(line,numeroAdmin))
  {
    std::string temp(line);
    int pos=temp.find("!kick");
    pos=temp.find_first_of("0123456789",pos+5);
    int end=temp.find_first_of(" ",pos);
    std::string numero=temp.substr(pos,end-pos);
    
    std::string frase("BanBot: kicking player number ");
    frase.append(numero);
    frase.append("...");
    server->say(frase,serverNumber);
    sleep(SOCKET_PAUSE);
    server->kick(numero,serverNumber);
  }
}

void Analyzer::mute(char* line)
{
  std::cout<<"[!] Mute";
  logger->timestamp();
  *logger<<"\n[!] Mute";
  //controllo se ho il giocatore e i suoi permessi, se la persona non è autorizzata non faccio nulla.
  std::string numeroAdmin;
  if (isAdminSay(line,numeroAdmin))
  {
    std::string temp(line);
    int pos=temp.find("!mute");
    pos=temp.find_first_of("0123456789",pos+5);
    int end=temp.find_first_of(" ",pos);
    std::string numero=temp.substr(pos,end-pos);
    
    std::string frase("BanBot: muting/unmuting player number ");
    frase.append(numero);
    frase.append("...");
    server->say(frase,serverNumber);
    sleep(SOCKET_PAUSE);
    server->mute(numero,serverNumber);
  }
}

void Analyzer::help(char* line)
{
  std::cout<<"[!] Help";
  logger->timestamp();
  *logger<<"\n[!] Help";
  std::string numero;
  if (isAdminSay(line,numero))
  {
    tell(COMMANDLIST,numero);
  }
}

void Analyzer::tell(std::string frase, std::string player)
{
  for (unsigned int i=0;i<frase.size();i+=130)
    {
      server->tell(frase.substr(i,130),player,serverNumber);
      sleep(SOCKET_PAUSE);
    }
}

void Analyzer::getDateAndTime(std::string &data,std::string &ora)
{
  time_t tempo;
  tempo=time(NULL);
  struct tm *tmp;
  tmp = localtime(&tempo);
  char outstr[20];
  strftime(outstr, sizeof(outstr), "%H:%M", tmp);
  ora=outstr;

  strftime(outstr, sizeof(outstr), "%F", tmp);
  data=outstr;
}

void Analyzer::buttaFuori(const std::vector<std::string> &reason, const std::string numero, const std::string nick)
{
  //è stato bannato, lo butto fuori
      std::cout<<"  [+] kick per ban.\n";
      *logger<<"  [+] kick per ban.\n";
      std::string frase("BanBot: kicking player number ");
      frase.append(numero);
      frase.append(", ");
      frase.append(nick);
      if (reason.size()>0)
      {
        frase.append(" banned for ");
        frase.append(reason[0]);
      }
      frase.append(".");
      server->say(frase,serverNumber);
      sleep(SOCKET_PAUSE);
      server->kick(numero,serverNumber);

      sleep(SOCKET_PAUSE);
      frase.erase();
      frase.append("Warning: the nick '");
      frase.append(nick);
      frase.append("' will be banned for an hour");
      server->say(frase,serverNumber);
}

bool Analyzer::nickIsBanned(const std::string &nick)
{
  std::string ora;
  std::string data;
  getDateAndTime(data,ora);

  std::string query("SELECT date,time FROM banned WHERE nick='");
  query.append(correggi(nick));
  query.append("';");
  std::vector<std::string> risultato=database->extractData(query);

  int oraAttuale=atoi(ora.substr(0,2).c_str());
  int minutoAttuale=atoi(ora.substr(3,5).c_str());
  for (unsigned int i=0;i<risultato.size();i+=2)
  {
    int oraBan=atoi(risultato[i+1].substr(0,2).c_str());
    int minutoBan=atoi(risultato[i+1].substr(3,5).c_str());
    int diff=(oraAttuale*60+minutoAttuale)-(oraBan*60+minutoBan);
    if (data.compare(risultato[i])==0 && diff>0 && diff<60) return true;
  }
  return false;
}

bool Analyzer::ipIsBanned(const std::string &ip)
{
  std::string ora;
  std::string data;
  getDateAndTime(data,ora);

  std::string query("SELECT date,time FROM banned WHERE ip='");
  query.append(ip);
  query.append("';");
  std::vector<std::string> risultato=database->extractData(query);

  int oraAttuale=atoi(ora.substr(0,2).c_str());
  int minutoAttuale=atoi(ora.substr(3,5).c_str());
  for (unsigned int i=0;i<risultato.size();i+=2)
  {
    int oraBan=atoi(risultato[i+1].substr(0,2).c_str());
    int minutoBan=atoi(risultato[i+1].substr(3,5).c_str());
    int diff=(oraAttuale*60+minutoAttuale)-(oraBan*60+minutoBan);
    if (data.compare(risultato[i])==0 && diff>0 && diff<60) return true;
  }
  return false;
}

std::string Analyzer::correggi(std::string stringa)
{
  unsigned int pos=0;
  bool nonFinito=true;
  while (nonFinito)
  {
    nonFinito=false;
    pos=stringa.find("'",pos);
    if (pos<stringa.size()) 
    {
      stringa=stringa.replace(pos,1,"''");
      pos+=2;
      nonFinito=true;
    }
  }
  return stringa;
}

//main loop
void Analyzer::main_loop()
{
  generalLog->open();
  std::cout<<"[OK] BanBot avviato.\n\n";
  *generalLog<<"[OK] BanBot avviato.\n\n";
  generalLog->close();
  while (true)
  {
    for (serverNumber=0;serverNumber<giocatori.size();serverNumber++)
    {
      //provo ad aprire il file e a riprendere dalla riga dove ero arrivato
      generalLog->open();
      std::cout<<"Provo ad aprire "<<files[serverNumber]<<"\n";
      generalLog->timestamp();
      *generalLog<<"\nProvo ad aprire "<<files[serverNumber]<<"\n";
      log.open(files[serverNumber].c_str());
      log.seekg(row[serverNumber]);
      if (log.is_open())
      {
        std::cout<<"  [OK] Aperto!\n";
        *generalLog<<"  [OK] Aperto!\n";
      }
      else 
      {
        std::cout<<"  [FAIL] Non sono riuscito ad aprirlo!\n";
        *generalLog<<"  [FAIL] Non sono riuscito ad aprirlo!\n";
      }
      if(backup->doJobs()) server->reload();
      generalLog->close();

      //se il file è aperto posso lavorare
      if (log.is_open())
      {
        logger->changePath(BotLogFiles[serverNumber]);
        logger->open();
        //il file è aperto, esamino le nuove righe (se ce ne sono)
        while (!log.eof())
        {
          //leggo una riga
          char line [1500];
          log.getline(line,1500,'\n');
          //se non è la fine del file, mi salvo la riga dove sono arrivato
          if (!log.eof()) row[serverNumber]=log.tellg();

          //comincio coi test
          if (isA(line, CLIENT_USER_INFO))
          {
            //ha passato il regex, è una clientUserinfo
            clientUserInfo(line);
          }
          else
          {
            //non ha passato il test, non è un clientUserinfo: provo con gli altri regex
            //controllo se è la connessione di un utente
            if (isA(line, CLIENT_CONNECT))
            {
              //è un clientconnect:
              clientConnect(line);
            }
            else
            {
              //non è un clientConnect, provo con gli altri regex_t
              //controllo se è una disconnessione
              if (isA(line, CLIENT_DISCONNECT))
              {
                //è un clientDisconnect
                clientDisconnect(line);
              }
              else
              {
                //non è neanche un clientDisconnect
                //controllo se è un comando di ban
                if (isA(line, BAN))
                {
                  //è una richiesta di ban:
                  ban(line);
                }
                else
                {
                  //non è una richiesta di ban...
                  //controllo se è l'initgame
                  if (isA(line, INITGAME))
                  {
                    //ok, è l'inizio di una nuova partita, resetto i player:
                    //elimino gli oggetti Player:
                    for (unsigned int i=0;i<giocatori[serverNumber].size();i++) delete giocatori[serverNumber][i];
                    //resetto il vector:
                    giocatori[serverNumber].clear();
                  }
                  else
                  {
                    //controllo se è la richiesta di un find
                    if (isA(line,FIND))
                    {
                      //è un find.
                      find(line);
                    }
                    else
                    {
                      //controllo se è una richiesta di unban
                      if (isA(line,UNBAN))
                      {
                        //è un comando di unban
                        unban(line);
                      }
                      else
                      {
                        if (isA(line,OP))
                        {
                          //è un comando di op
                          op(line);
                        }
                        else
                        {
                          if (isA(line,DEOP))
                          {
                            //è un comando di deop
                            deop(line);
                          }
                          else
                          {
                            if (isA(line,HELP))
                            {
                              //è un help
                              help(line);
                            }
                            else
                            {
                              if (isA(line,FINDOP))
                              {
                                //è un findop
                                findOp(line);
                              }
                              else
                              {
                                if (isA(line,KICK))
                                {
                                  //è un kick
                                  kick(line);
                                }
                                else
                                {
                                  if (isA(line,MUTE))
                                  {
                                    //è un mute
                                    mute(line);
                                  }
                                  else
                                  {
                                    if (isA(line,UNMUTE))
                                    {
                                      //è un unmute
                                      mute(line);
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
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      else
        row[serverNumber]=0;//se non riesco ad aprire il file, ricomincio dalla prima riga
      //chiudo il file e lascio passare un po' di tempo
      log.close();
      logger->close();
    }
    sleep(TIME_SLEEPING);
  }
}
#endif