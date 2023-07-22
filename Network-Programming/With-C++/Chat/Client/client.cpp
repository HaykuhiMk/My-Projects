#include "client.h"
#include <conio.h>

Client::Client(const char* serverIP, const char* port)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) 
    {
        std::cerr << "Failed to initialize!" << std::endl;
        exit(EXIT_FAILURE);
    }
#endif
    std::cout << "Configuring remote address..." << std::endl;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.ai_socktype = SOCK_STREAM;
    struct addrinfo *peerAddress;
    if (getaddrinfo(serverIP, port, &clientAddr, &peerAddress)) 
    {
        perror("getaddrinfo() failed!");
        exit(EXIT_FAILURE);
    }
    std::cout << "Remote address is: " << std::endl;
    char addressBuffer[100];
    char serviceBuffer[100];
    getnameinfo(peerAddress->ai_addr, peerAddress->ai_addrlen,
        addressBuffer, sizeof(addressBuffer),
        serviceBuffer, sizeof(serviceBuffer),
        NI_NUMERICHOST);
    std::cout << addressBuffer << " " << serviceBuffer << std::endl;
    std::cout << "Creating socket..." << std::endl;
    clientSocket = socket(peerAddress->ai_family,
        peerAddress->ai_socktype, peerAddress->ai_protocol);
    if (!ISVALIDSOCKET(clientSocket)) 
    {
        perror("Error creating socket!");
        exit(EXIT_FAILURE);
    }
    std::cout << "Connecting..." << std::endl;
    if (connect(clientSocket, peerAddress->ai_addr, peerAddress->ai_addrlen)) 
    {
        perror("Error connecting!");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(peerAddress);
    std::cout << "Connected." << std::endl;
}

Client::~Client() 
{
    std::cout << "Closing socket..." << std::endl;
    CLOSESOCKET(clientSocket);
#ifdef _WIN32
    WSACleanup();
#else
    close(clientSocket);
#endif
}

void Client::sendMessage() 
{
    std::cout << "To send data, enter text followed by enter." << std::endl;
    while(1) 
    {
        fd_set reads;       //store socket set
        FD_ZERO(&reads);        //zero it
        FD_SET(clientSocket, &reads);        //set it 
#ifndef _WIN32
        FD_SET(0, &reads);      
#endif
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        if (select(clientSocket+1, &reads, 0, 0, &timeout) < 0)      // check to see whether the socket is set in reads
        {
            perror("Error selecting!");
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(clientSocket, &reads)) 
        {
            char read[4096];
            int bytesReceived = recv(clientSocket, read, 4096, 0);
            if (bytesReceived < 1) 
            {
                std::cout << "Connection closed by peer." << std::endl;
                break;
            }
            std::cout << "Received " << bytesReceived << " : " << read << std::endl;
        }
#ifdef _WIN32
        if(_kbhit()) {
#else
        if(FD_ISSET(0, &reads)) {
#endif
            char read[4096];
            if (!fgets(read, 4096, stdin)) break;
            std::cout << "Sending: " << read << std::endl;
            int bytesSent = send(clientSocket, read, strlen(read), 0);
            std::cout << "Sent " << bytesSent << " bytes." << std::endl;
        }
    } //end while(1)  
}
