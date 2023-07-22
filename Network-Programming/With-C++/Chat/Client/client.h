#include "conf.h"
#include <string>
#include <iostream>
#ifndef CLIENT_H
#define CLIENT_H

class Client 
{
private:
    SOCKET clientSocket;
    struct addrinfo clientAddr;

public:
    Client(const char*, const char*);
    ~Client();
    void sendMessage();
};

#endif      //CLIENT_H