#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

class TCPServer {
   private:
    int server_fd;
    int port;
    struct sockaddr_in address;

   public:
    TCPServer(int port);

    ~TCPServer();

    bool start();

    void run();

   private:
    void handleClient(int client_socket);
};