#include "server.h"

Server::Server() 
{
#ifdef _WIN32
    WSADATA wsaData;
    //initialization the Winsock library
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))       
    {
        std::cerr << "Failed to initialize!" << std::endl;
        exit(EXIT_FAILURE);
    }
#endif  //_WIN32
    std::cout << "Configuring local address..." << std::endl;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.ai_family = AF_INET;
    serverAddr.ai_socktype = SOCK_STREAM;
    serverAddr.ai_flags = AI_PASSIVE;
    struct addrinfo *bindAddress;
    getaddrinfo(0, "8080", &serverAddr, &bindAddress);
    std::cout << "Creating socket..." << std::endl;
    serverSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, 
        bindAddress->ai_protocol);
    if (!ISVALIDSOCKET(serverSocket)) 
    {
        perror("Error creating socket!");
        exit(EXIT_FAILURE);
    }
    std::cout << "Binding socket to local address..." << std::endl;
    if (bind(serverSocket, bindAddress->ai_addr, bindAddress->ai_addrlen)) 
    {
        perror("Error binding!");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(bindAddress);
}

Server::~Server() 
{
    std::cout << "Closing listening socket..." << std::endl;
    CLOSESOCKET(serverSocket);
#ifdef _WIN32
    WSACleanup();
#else
    close(serverSocket);
#endif
    std::cout << "Finished." << std::endl;
}

void Server::run() 
{
    std::cout << "Listening.." << std::endl;
    if (listen(serverSocket, 10) < 0) 
    {
        perror("Error listening!");
        exit(EXIT_FAILURE);
    }
    fd_set master;
    FD_ZERO(&master);
    FD_SET(serverSocket, &master);
    SOCKET maxSocket = serverSocket;
    std::cout << "Waiting for connections..." << std::endl;
    while (true) 
    {
        fd_set reads;       //store socket set
        reads = master;
        if (select(maxSocket+1, &reads, 0, 0, 0) < 0) 
        {
            perror("Error selecting!");
            exit(EXIT_FAILURE);
        }
        SOCKET i;
        for (i = 0; i <= maxSocket; ++i) 
        {
            if (FD_ISSET(i, &reads)) 
            {
                if (i == serverSocket) 
                {
                    struct sockaddr_storage clientAddress;
                    socklen_t clientLen = sizeof(clientAddress);
                    SOCKET socketClient = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientLen);
                    if (!ISVALIDSOCKET(socketClient)) 
                    {
                        perror("Error selecting!");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(socketClient, &master);
                    if (socketClient > maxSocket)
                    {
                        maxSocket = socketClient;
                    }
                    char addressBuffer[NI_MAXHOST];
                    getnameinfo((struct sockaddr*)&clientAddress, clientLen,
                            addressBuffer, sizeof(addressBuffer), 0, 0,
                            NI_NUMERICHOST);
                    std::cout << "New connection from " << addressBuffer << std::endl;
                } 
                else 
                {
                    char read[1024];
                    int bytesReceived = recv(i, read, sizeof(read), 0);
                    if (bytesReceived < 1) 
                    {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    SOCKET j;
                    for (j = 0; j <= maxSocket; ++j) 
                    {
                        if (FD_ISSET(j, &master) && j != serverSocket && j != i)
                        {
                            send(j, read, bytesReceived, 0);
                        }
                    }
                }
            }     
        }   
    }  
}        
