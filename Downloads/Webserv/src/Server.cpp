#include "Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

int Server::getServerSocket() const
{
    return serverSocket;
}

int Server::getPort() const
{
    return port;
}

void Server::createSocket()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        throw std::runtime_error("Error : Failed In creating sockets ");
    }
}

void Server::bindSocket()
{
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        throw std::runtime_error("Error brother: Failed to bind sockets");
    }
}

void Server::listenSocket()
{
    if (listen(serverSocket, 3) < 0)
    {
        throw std::runtime_error("Error brother: Failed to listen sockets");
    }
}

bool Server::start()
{
    try
    {
        createSocket();
        bindSocket();
        listenSocket();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        if (serverSocket >= 0)
        {
            close(serverSocket);
        }
        return false;
    }

    std::cout << "Server started on port " << port << std::endl;
    return true;
}

Server::Server(int port) : port(port), serverSocket(-1)
{
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
}
