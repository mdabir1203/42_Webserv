#include "../inc/ServerManager.hpp"
#include <iostream>

bool ServerManager::initialize() {
    // logic for init servers
    // placeholder for cgi config
    servers.push_back(Server(4242));
    return true;
}


void ServerManager::run() {
    // Main server loop 
    // 
    std::cout << "uknowWho Server is running high" << std::endl;
    while (true) {
        // Accept incoming connections
        // Handle the connection
        // Parse the HTTP request
        // Handle the request
        // Send the response
    }
}