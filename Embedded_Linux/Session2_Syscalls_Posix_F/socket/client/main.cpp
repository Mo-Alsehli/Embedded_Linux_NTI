#include "client.h"

int main() {
    TCPClient client;

    if (!client.connect_to_server("127.0.0.1", 8080)) {
        return -1;
    }

    client.run();
    return 0;
}