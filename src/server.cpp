
// server.cpp
#include "server.hpp"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const char* HttpServer::DEFAULT_FILE = "index.html";

HttpServer::HttpServer(const std::vector<int>& ports, const std::map<std::string, std::string>& hostnames)
    : ports_(ports), hostnames_(hostnames) {
    // Initialize default routes
    RouteInfo defaultRoute = {".", std::vector<std::string>()};
    defaultRoute.allowedMethods.push_back("GET");
    defaultRoute.allowedMethods.push_back("POST");
    routes_["/"] = defaultRoute;
}

void HttpServer::start() {
    for (std::vector<int>::const_iterator it = ports_.begin(); it != ports_.end(); ++it) {
        startServer(*it);
    }
}

void HttpServer::startServer(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return;
    }

    if (listen(sockfd, 3) < 0) {
        perror("listen");
        return;
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_sockfd < 0) {
            perror("accept");
            continue;
        }

        handleRequest(client_sockfd);

        close(client_sockfd);
    }
}

void HttpServer::handleRequest(int client_sockfd) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("recv");
        return;
    }
    buffer[bytes_received] = '\0';

    std::string request(buffer);
    std::cout << "Received request: " << request << std::endl;

    std::string method, path, version;
    std::istringstream request_stream(request);
    request_stream >> method >> path >> version;

    if (method.empty() || path.empty() || version.empty()) {
        sendError(client_sockfd, 400, "Bad Request");
        return;
    }

    // Find the appropriate route
    std::string directory = "/admin";
    std::vector<std::string> allowedMethods;
    for (std::map<std::string, RouteInfo>::const_iterator it = routes_.begin(); it != routes_.end(); ++it) {
        if (path.find(it->first) == 0) {
            directory = it->second.directory;
            allowedMethods = it->second.allowedMethods;
            break;
        }
    }

    // Check if the method is allowed
    bool methodAllowed = false;
    for (std::vector<std::string>::const_iterator it = allowedMethods.begin(); it != allowedMethods.end(); ++it) {
        if (*it == method) {
            methodAllowed = true;
            break;
        }
    }

    if (!methodAllowed) {
        sendError(client_sockfd, 405, "Method Not Allowed");
        return;
    }

    if (method == "GET") {
        handleGetRequest(client_sockfd, directory + path);
    } else if (method == "POST") {
        if (request.size() > MAX_BODY_SIZE) {
            sendError(client_sockfd, 413, "Payload Too Large");
        } else {
            handlePostRequest(client_sockfd, request);
        }
    } else {
        sendError(client_sockfd, 501, "Not Implemented");
    }
}

void HttpServer::handleGetRequest(int client_sockfd, const std::string& path) {
    std::string filePath = path;
    if (filePath[filePath.size() - 1] == '/') {
        filePath += DEFAULT_FILE;
    }

    std::ifstream file(filePath.c_str(), std::ios::binary);
    if (file.is_open()) {
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(client_sockfd, response.c_str(), response.size(), 0);

        char buffer[BUFFER_SIZE];
        while (file.read(buffer, sizeof(buffer)).gcount() > 0) {
            send(client_sockfd, buffer, file.gcount(), 0);
        }
    } else {
        sendError(client_sockfd, 404, "Not Found");
    }
}

void HttpServer::handlePostRequest(int client_sockfd, const std::string& request) {
    size_t body_pos = request.find("\r\n\r\n");
    std::string body = (body_pos != std::string::npos) ? request.substr(body_pos + 4) : "";

    std::cout << "Received POST body: " << body << std::endl;

    std::string response = "HTTP/1.1 200 OK\r\n\r\nPOST request processed successfully!";
    send(client_sockfd, response.c_str(), response.size(), 0);
}

void HttpServer::sendError(int client_sockfd, int code, const std::string& message) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << code << " " << message << "\r\n\r\n";
    responseStream << "<html><body><h1>" << code << " " << message << "</h1></body></html>";
    std::string response = responseStream.str();
    send(client_sockfd, response.c_str(), response.size(), 0);
}
