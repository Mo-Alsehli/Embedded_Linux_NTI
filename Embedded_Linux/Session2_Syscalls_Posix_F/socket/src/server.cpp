#include "server.h"

TCPServer::TCPServer(int port) : port(port), server_fd(-1) {}

TCPServer::~TCPServer() {
    if (server_fd != -1) {
        close(server_fd);
    }
}

bool TCPServer::start() {
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    // // Set socket options to reuse address
    // int opt = 1;
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    //     std::cerr << "Setsockopt failed" << std::endl;
    //     return false;
    // }

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    address.sin_port = htons(port);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return false;
    }

    // Start listening (max 3 pending connections)
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }

    std::cout << "Server listening on port " << port << std::endl;
    return true;
}

void TCPServer::run() {
    int addrlen = sizeof(address);

    while (true) {
        // Accept incoming connection
        int client_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen);
        if (client_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "Client connected" << std::endl;
        handleClient(client_socket);
    }
}

void TCPServer::handleClient(int client_socket) {
    char buffer[1024] = {0};

    while (true) {
        // Read message from client
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        buffer[bytes_read] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        // Echo message back to client
        std::string response = "Echo: " + std::string(buffer);
        send(client_socket, response.c_str(), response.length(), 0);

        // Clear buffer
        memset(buffer, 0, sizeof(buffer));
    }

    close(client_socket);
}