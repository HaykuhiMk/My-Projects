#include "conf.h"
#include <string>
#include <iostream>
#ifndef SERVER_H
#define SERVER_H

class Server 
{
private:
    SOCKET serverSocket;
    struct addrinfo serverAddr;

public:
    Server();
    ~Server();
    void startListening();
};

#endif      //SERVER_H