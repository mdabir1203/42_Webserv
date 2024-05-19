#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

Server::Server(int port) : port(port), serverSocket(-1) {
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
}


bool Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Socket gets a creation error " << std::endl;
        return false;
    }

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Socket gets a binding error " << std::endl;
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Failed to listen on port " << port << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << "Server started on Port *_* " << port << std::endl;
    return true;
    
}


