#ifndef CLIENT_H
#define CLIENT_H

#include "libs.h"

#include <pthread.h>

struct Client {
    SOCKET clientSocket;
    struct addrinfo clientAddr;
	char clientName[MAX_CLIENT_NAME_LENGTH];
};

struct ReceiveThreadArgs {
    struct Client* client;
};

void clientInit(struct Client* client, const char* serverIP, const char* port);
void clientCleanup(struct Client* client);
void clientSendMessage(struct Client* client);
void* receiveMessages(void*);


#endif	//CLIENT_H

