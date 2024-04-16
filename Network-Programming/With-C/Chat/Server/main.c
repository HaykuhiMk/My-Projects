#include "server.h"
#include <string.h>

int main() {
    struct Server server;
    serverInit(&server);
    serverRun(&server);
    serverCleanup(&server);
    return 0;
}
