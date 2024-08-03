#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <cstdio>
#include <fstream>

#define BUFFER_SIZE 1024
#define MAX_BODY_SIZE 1024

class HttpServer {
public:
    HttpServer(const std::vector<int>& ports, const std::map<std::string, std::string>& hostnames)
        : ports_(ports), hostnames_(hostnames) {}

    void start() {
        for (size_t i = 0; i < ports_.size(); ++i) {
            int port = ports_[i];
            startServer(port);
        }
    }

private:
    void startServer(int port) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("socket");
            return;
        }

        struct sockaddr_in server_addr;
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

    void handleRequest(int client_sockfd) {
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            perror("recv");
            return;
        }

        std::string request(buffer, bytes_received);
        std::cout << "Received request: " << request << std::endl;

        std::vector<std::string> request_lines;
        size_t pos = 0;
        while ((pos = request.find("\r\n")) != std::string::npos) {
            request_lines.push_back(request.substr(0, pos));
            request.erase(0, pos + 2);
        }

        if (request_lines.empty()) {
            sendError(client_sockfd, 400, "Bad Request");
            return;
        }

        std::string method = request_lines[0].substr(0, request_lines[0].find(' '));
        std::string path = request_lines[0].substr(request_lines[0].find(' ') + 1);
        path = path.substr(0, path.find(' '));

        if (method == "GET") {
            handleGetRequest(client_sockfd, path);
        } else if (method == "POST") {
            if (request.size() > MAX_BODY_SIZE) {
                sendError(client_sockfd, 413, "Payload Too Large");
            } else {
                handlePostRequest(client_sockfd, request);
            }
        } else {
            sendError(client_sockfd, 405, "Method Not Allowed");
        }
    }

void handleGetRequest(int client_sockfd, const std::string& path) {
    std::string response;
    if (path == "/") {
        response = "HTTP/1.1 200 OK\r\n\r\nHello, GET!";
    } else {
        std::string filePath = "." + path; // Use std::string for concatenation
        std::ifstream file(filePath.c_str()); // Convert to C-string for ifstream
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            response = "HTTP/1.1 200 OK\r\n\r\n" + buffer.str();
        } else {
            sendError(client_sockfd, 404, "Not Found");
            return;
        }
    }
    send(client_sockfd, response.c_str(), response.size(), 0);
}

    void handlePostRequest(int client_sockfd, const std::string& request) {
        size_t body_pos = request.find("\r\n\r\n");
        std::string body = (body_pos != std::string::npos) ? request.substr(body_pos + 4) : "";

        std::cout << "Received POST body: " << body << std::endl;

        std::string response = "HTTP/1.1 200 OK\r\n\r\nHello, POST!";
        send(client_sockfd, response.c_str(), response.size(), 0);
    }

    void sendError(int client_sockfd, int code, const std::string& message) {
        std::ostringstream responseStream;
        responseStream << "HTTP/1.1 " << code << " " << message << "\r\n\r\n";
        responseStream << "<html><body><h1>" << code << " " << message << "</h1></body></html>";
        std::string response = responseStream.str();
        send(client_sockfd, response.c_str(), response.size(), 0);
    }

    std::vector<int> ports_;
    std::map<std::string, std::string> hostnames_;
};

int main() {
    std::vector<int> ports;
    ports.push_back(5500);
    ports.push_back(5501);

    std::map<std::string, std::string> hostnames;
    hostnames["example.com"] = "127.0.0.1";

    HttpServer server(ports, hostnames);
    server.start();
    while (true) {
        // Keep the main thread alive
    }
    return 0;
}