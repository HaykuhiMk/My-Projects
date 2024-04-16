#include "client.h"

void clientInit(struct Client* client, const char* serverIP, const char* port) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        fprintf(stderr, "Failed to initialize!\n");
        exit(EXIT_FAILURE);
    }
#endif

    printf("Configuring remote address...\n");
    memset(&client->clientAddr, 0, sizeof(client->clientAddr));
    client->clientAddr.ai_socktype = SOCK_STREAM;
    struct addrinfo* peerAddress;
    if (getaddrinfo(serverIP, port, &client->clientAddr, &peerAddress)) {
        perror("getaddrinfo() failed!");
        exit(EXIT_FAILURE);
    }
    printf("Remote address is: \n");
    char addressBuffer[100];
    char serviceBuffer[100];
    getnameinfo(peerAddress->ai_addr, peerAddress->ai_addrlen,
        addressBuffer, sizeof(addressBuffer),
        serviceBuffer, sizeof(serviceBuffer),
        NI_NUMERICHOST);
    printf("%s %s\n", addressBuffer, serviceBuffer);
    printf("Creating socket...\n");
    client->clientSocket = socket(peerAddress->ai_family,
        peerAddress->ai_socktype, peerAddress->ai_protocol);
    if (!ISVALIDSOCKET(client->clientSocket)) {
        perror("Error creating socket!");
        exit(EXIT_FAILURE);
    }
    printf("Connecting...\n");
    if (connect(client->clientSocket, peerAddress->ai_addr, peerAddress->ai_addrlen)) {
        perror("Error connecting!");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(peerAddress);
    printf("Connected.\n");
}

void clientCleanup(struct Client* client) {
    printf("Closing socket...\n");
    CLOSESOCKET(client->clientSocket);
#ifdef _WIN32
    WSACleanup();
#else
    close(client->clientSocket);
#endif
}

void clientSendMessage(struct Client* client) {
    pthread_t recvThread;
    struct ReceiveThreadArgs recvArgs = { client };
    if (pthread_create(&recvThread, NULL, receiveMessages, (void*)&recvArgs) != 0) {
        perror("Error creating receive thread");
        return;
    }
    while (1) {
        char read[4096];
        if (!fgets(read, sizeof(read), stdin)) break;
        read[strcspn(read, "\n")] = '\0'; 
        int bytesSent = send(client->clientSocket, read, strlen(read), 0);
        if (bytesSent <= 0) {
            printf("Error sending data to server.\n");
            break;
        }
    }
    if (pthread_join(recvThread, NULL) != 0) {
        perror("Error joining receive thread");
        return;
    }
}


void* receiveMessages(void* args) {
    struct ReceiveThreadArgs* recvArgs = (struct ReceiveThreadArgs*)args;
    struct Client* client = recvArgs->client;
    char buffer[4096];
    int bytesReceived;
    while (1) {
        bytesReceived = recv(client->clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            printf("Connection closed by server.\n");
            break;
        }
        buffer[bytesReceived] = '\0';
        printf("%s\n", buffer); 
    }
    clientCleanup(client);
    return NULL;
}