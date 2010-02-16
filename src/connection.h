#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
using std::string;//using namespace std;

class connection( string ip, int port )
{
    public:
        connection();
        ~connection();
        
    private:
        string ip;
        int port;   
};




#endif  //CONNECTION_H