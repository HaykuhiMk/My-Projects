#include "server.h"

void addClient(struct Server* server, SOCKET socket, const char* clName) {
    if (server->numClients < MAX_CLIENTS) {
        server->clients[server->numClients].clientSocket = socket;
        strncpy(server->clients[server->numClients].clientName, clName, MAX_CLIENT_NAME_LENGTH - 1);
        server->clients[server->numClients].clientName[MAX_CLIENT_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
        server->numClients++;
    } else {
        fprintf(stderr, "Maximum number of clients reached!\n");
    }
}

void broadcastMessage(struct Server* server, int senderSocket, const char* message) {
    char formattedMessage[MAX_MESSAGE_SIZE];
    const char* senderName = getClientName(server, senderSocket);
    snprintf(formattedMessage, sizeof(formattedMessage), "%s: %s", senderName, message);
    for (int i = 0; i < server->numClients; ++i) {
        if (server->clients[i].clientSocket != senderSocket) {
            if (send(server->clients[i].clientSocket, formattedMessage, strlen(formattedMessage), 0) < 0) {
                perror("Error broadcasting message to a client");
            }
        }
    }
}

const char* getClientName(struct Server* server, SOCKET clientSocket) {
    for (int i = 0; i < server->numClients; ++i) {
        if (server->clients[i].clientSocket == clientSocket) {
            return server->clients[i].clientName;
        }
    }
    return "";
}


void serverInit(struct Server* server) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        fprintf(stderr, "Failed to initialize!\n");
        exit(EXIT_FAILURE);
    }
#endif
    printf("Configuring local address...\n");
    memset(&server->serverAddr, 0, sizeof(server->serverAddr));
    server->serverAddr.ai_family = AF_INET;
    server->serverAddr.ai_socktype = SOCK_STREAM;
    server->serverAddr.ai_flags = AI_PASSIVE;
	server->numClients = 0;
    struct addrinfo* bindAddress;
    getaddrinfo(0, "8080", &server->serverAddr, &bindAddress);
    printf("Creating socket...\n");
    server->serverSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
    if (!ISVALIDSOCKET(server->serverSocket)) {
        perror("Error creating socket!");
        exit(EXIT_FAILURE);
    }
    printf("Binding socket to local address...\n");
    if (bind(server->serverSocket, bindAddress->ai_addr, bindAddress->ai_addrlen)) {
        perror("Error binding!");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(bindAddress);
}

void serverCleanup(struct Server* server) {
    printf("Closing listening socket...\n");
    CLOSESOCKET(server->serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif

    printf("Finished.\n");
}

void handleNewClient(struct Server* server, SOCKET clientSocket, struct sockaddr_storage* clientAddress, socklen_t clientLen) {
    const char* message = "Please provide your name.";
    if (send(clientSocket, message, strlen(message), 0) < 0) {
        perror("Error sending message to client!");
        CLOSESOCKET(clientSocket);
        return;
    }
    char clName[100];
    int bytesReceived = recv(clientSocket, clName, sizeof(clName) - 1, 0);
    if (bytesReceived <= 0) {
        perror("Error receiving client name!");
        CLOSESOCKET(clientSocket);
        return;
    }
    clName[bytesReceived] = '\0';
    printf("New connection from %s\n", clName);
    char addressBuffer[NI_MAXHOST];
    getnameinfo((struct sockaddr*)clientAddress, clientLen,
                addressBuffer, sizeof(addressBuffer), 0, 0,
                NI_NUMERICHOST);
    addClient(server, clientSocket, clName);
    char notification[256];
    snprintf(notification, sizeof(notification), "%s has joined the chat.\n", clName);
    broadcastMessage(server, server->numClients - 1, notification);
}


void serverRun(struct Server* server) {
    printf("Listening...\n");
    if (listen(server->serverSocket, 10) < 0) {
        perror("Error listening!");
        exit(EXIT_FAILURE);
    }
    fd_set master;
    FD_ZERO(&master);
    FD_SET(server->serverSocket, &master);
    SOCKET maxSocket = server->serverSocket;
    printf("Waiting for connections...\n");
    while (1) {
        fd_set reads;
        reads = master;
        if (select(maxSocket + 1, &reads, NULL, NULL, NULL) < 0) {
            perror("Error selecting!");
            exit(EXIT_FAILURE);
        }
        for (SOCKET i = 1; i <= maxSocket; ++i) {
            if (FD_ISSET(i, &reads)) {
                if (i == server->serverSocket) {
                    struct sockaddr_storage clientAddress;
                    socklen_t clientLen = sizeof(clientAddress);
                    SOCKET sockCl = accept(server->serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
                    if (!ISVALIDSOCKET(sockCl)) {
                        perror("Error accepting!");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(sockCl, &master);
                    if (sockCl > maxSocket) {
                        maxSocket = sockCl;
                    }
                    handleNewClient(server, sockCl, &clientAddress, clientLen);
                } else {
                    char read[1024];
                    int bytesReceived = recv(i, read, sizeof(read), 0);
                    if (bytesReceived <= 0) {
                        if (bytesReceived == 0) {
                        } else {
                            perror("Error receiving data from client!");
                        }
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    read[bytesReceived] = '\0';
                    broadcastMessage(server, i, read);
                }
            }
        }
    }
}
