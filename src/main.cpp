#include <iostream>
#include <string>
#include "db.h"
#include "sqlite3.h"

using namespace std;

/*
void config()
{
    std::cout<<"
    
    
}*/

int main(){

    //sqlite3 *database = config();
    db d;
    string a, b;
    a.append( "123" );
    b.append("asder");
    d.checkBanGuid( a, b );
    

}