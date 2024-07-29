#include "server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <algorithm>
#include <sys/select.h>
#include <map>
#include <vector>


int Client::getSocket() const
{
    return client_socket;
}

Client::Client(int socket) : client_socket(socket) {}

bool Client::handleRequest() {
    // Implement req handling logic
    char buffer[1024];
    ssize_t bytesread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytesread <= 0) {
        return false;
    }
    // Process req and send response
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nCheckMeCrazyUknow";
    send(client_socket, response.c_str(), response.size(), 0);
    return true;
}

HTTPServer::HTTPServer() : server_socket(-1) {}

HTTPServer::~HTTPServer() {
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;
    }
    closeSocket(server_socket);
}

bool HTTPServer::initialize() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(5500);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return false;
    }

    // Set server socket to non-blocking
    int flags = fcntl(server_socket, F_GETFL, 0);
    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);

    return true;
}

void HTTPServer::acceptNewClient() {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket >= 0) {
        // Set client socket to non-blocking
        int flags = fcntl(client_socket, F_GETFL, 0);
        fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

        clients[client_socket] = new Client(client_socket);
    }
}

void HTTPServer::handleClients() {
    std::vector<int> to_remove;

    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (!it->second->handleRequest()) {
            to_remove.push_back(it->first);
        }
    }

    for (size_t i = 0; i < to_remove.size(); ++i) {
        delete clients[to_remove[i]];
        clients.erase(to_remove[i]);
    }
}

int HTTPServer::selectCall(fd_set& read_fds) {
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);

    int max_fd = server_socket;
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        int client_socket = it->first;
        FD_SET(client_socket, &read_fds);
        if (client_socket > max_fd) {
            max_fd = client_socket;
        }
    }

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    select(max_fd + 1, &read_fds, NULL, NULL, &tv);
    return max_fd;
}



void HTTPServer::run() {
    while (true) {
        fd_set read_fds;
        selectCall(read_fds);

        if (FD_ISSET(server_socket, &read_fds)) {
            acceptNewClient();
        }

        handleClients();
    }
}

void HTTPServer::closeSocket(int socket) {
    if (socket != -1) {
        close(socket);
    }
}
