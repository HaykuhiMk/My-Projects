#ifndef SERVER_H
#define SERVER_H

#include "../Client/client.h"
#include "libs.h"

struct Server {
    SOCKET serverSocket;
    struct addrinfo serverAddr;
	struct Client clients[MAX_CLIENTS];
    int numClients;
};

void serverInit(struct Server*);
void serverCleanup(struct Server*);
void serverRun(struct Server*);
void addClient(struct Server*, SOCKET, const char*);
void broadcastMessage(struct Server* server, int senderIndex, const char* message);
const char* getClientName(struct Server* server, SOCKET clientSocket);
void handleNewClient(struct Server*, SOCKET, struct sockaddr_storage*, socklen_t);


#endif	//SERVER_H
