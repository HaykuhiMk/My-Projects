#ifndef SERVER_H
#define SERVER_H
#include "conf.h"
#include <string>
#include <iostream>

class Server 
{
public:
    Server();
    ~Server();
    void run();

private:
    SOCKET serverSocket;
    struct addrinfo serverAddr;

};

#endif      //SERVER_H
