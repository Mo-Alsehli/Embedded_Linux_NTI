#include "server.h"

int main() {
    TCPServer server(8080);

    if (!server.start()) {
        return -1;
    }

    server.run();
    return 0;
}