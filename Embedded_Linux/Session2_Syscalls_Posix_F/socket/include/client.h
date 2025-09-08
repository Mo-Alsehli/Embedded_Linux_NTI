#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

class TCPClient {
   private:
    int sock;
    struct sockaddr_in server_addr;

   public:
    TCPClient();

    ~TCPClient();

    bool connect_to_server(const std::string& server_ip, int port);

    bool send_message(const std::string& message);

    std::string receive_message();

    void run();
};