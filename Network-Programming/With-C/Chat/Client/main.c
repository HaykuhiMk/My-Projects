#include "client.h"

int main() {
    const char* serverIP = "127.0.0.1"; 
    const char* port = "8080"; 
    struct Client client;
    clientInit(&client, serverIP, port);
    clientSendMessage(&client);
    clientCleanup(&client);
    return 0;
}
