#include "client.h"

TCPClient::TCPClient() : sock(-1) {}

TCPClient::~TCPClient() {
    if (sock != -1) {
        close(sock);
    }
}

bool TCPClient::connect_to_server(const std::string& server_ip, int port) {
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convert IP address from text to binary
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        return false;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return false;
    }

    std::cout << "Connected to server" << std::endl;
    return true;
}

bool TCPClient::send_message(const std::string& message) {
    ssize_t bytes_sent = send(sock, message.c_str(), message.length(), 0);
    return bytes_sent > 0;
}

std::string TCPClient::receive_message() {
    char buffer[1024] = {0};
    ssize_t bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        return std::string(buffer);
    }

    return "";
}

void TCPClient::run() {
    std::string message;

    while (true) {
        std::cout << "Enter message (or 'quit' to exit): ";
        std::getline(std::cin, message);

        if (message == "quit") {
            break;
        }

        if (!send_message(message)) {
            std::cerr << "Send failed" << std::endl;
            break;
        }

        std::string response = receive_message();
        if (!response.empty()) {
            std::cout << "Server response: " << response << std::endl;
        } else {
            std::cerr << "Server disconnected" << std::endl;
            break;
        }
    }
}