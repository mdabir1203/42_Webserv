#include "../inc/ServerManager.hpp"
#include <iostream>
#include <sys/select.h>

bool ServerManager::initialize() {
    // logic for init servers
    // placeholder for cgi config
    servers.push_back(Server(4242));
    return true;
}


void ServerManager::run() {
    // Main server loop 
    // 
    fd_set readfds;
    int maxfd = 0;

    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
        if (!it->start()) {
            std::cerr << "Failed to start server on port " << it->getPort() << std::endl;
            return;
        }
        if (it->getServerSocket() > maxfd) {
            maxfd = it->getServerSocket();
        }

        while (true) {
            FD_ZERO(&readfds);
            for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) {
                FD_SET(it->getServerSocket(), &readfds);
            }

            int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
            if (activity < 0) {
                std::cerr << "Error in select()" << std::endl;
                continue;
            }
            for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
            {
                if (FD_ISSET(it->getServerSocket(), &readfds))
                {
                    // Handle new connection
                    int newSocket = accept(it->getServerSocket(), NULL, NULL);
                    if (newSocket < 0)
                    {
                        std::cerr << "Failed to accept connection" << std::endl;
                        continue;
                    }
                    // Placeholder: Handle the new connection
                    std::cout << "New connection accepted on port " << it->getPort() << std::endl;
                }
            }
        }
}


void ServerManager::addServer(const Server& server) {
    servers.push_back(server);
}