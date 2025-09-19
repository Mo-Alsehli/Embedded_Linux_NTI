#include "client.h"

int main() {
    TCPClient client;

    if (!client.connect_to_server("192.168.8.82", 8080)) {
        return -1;
    }

    client.run();
    return 0;
}