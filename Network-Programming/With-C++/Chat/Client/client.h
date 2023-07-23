#ifndef CLIENT_H
#define CLIENT_H
#include "conf.h"
#include <string>
#include <iostream>

class Client 
{
public:
    Client(const char*, const char*);
    ~Client();
    void sendMessage();

private:
    SOCKET clientSocket;
    struct addrinfo clientAddr;

};

#endif      //CLIENT_H
