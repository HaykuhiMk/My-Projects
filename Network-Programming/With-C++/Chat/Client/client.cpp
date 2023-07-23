#include "client.h"
#include <thread>

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
    // Separate thread for receiving messages
    // This thread is responsible for continuously receiving data from a client socket
    std::thread receiverThread([&]() 
    {
        while (true) 
        {
            char read[4096];
            int bytesReceived = recv(clientSocket, read, 4096, 0);
            if (bytesReceived <= 0) 
            {
                std::cout << "Connection closed by peer." << std::endl;
                break;
            }
            read[bytesReceived] = '\0'; // Null-terminate the received data
            std::cout << "Received " << bytesReceived << " bytes: " << read << std::endl;
        }
    });
    while (true) 
    {
        char read[4096];
        if (!fgets(read, sizeof(read), stdin)) break;
        read[strcspn(read, "\n")] = '\0'; // Remove trailing newline character
        std::cout << "Sending: " << read << std::endl;
        int bytesSent = send(clientSocket, read, strlen(read), 0);
        std::cout << "Sent " << bytesSent << " bytes." << std::endl;
    }

    // Cleanup and join the receiver thread
    // The main thread essentially waits for the receiverThread to finish before proceeding. 
    receiverThread.join();
}
