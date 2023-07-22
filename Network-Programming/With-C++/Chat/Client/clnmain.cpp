#include "client.h"
#include <iostream>
#include <string>

int main() {
    std::string serverIP;
    std::string port;

    std::cout << "Enter the server IP address: ";
    std::cin >> serverIP;
    std::cout << "Enter the server port: ";
    std::cin >> port;

    Client client(serverIP.c_str(), port.c_str());
    client.sendMessage();

    return 0;
}
