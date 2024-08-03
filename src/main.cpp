#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5500
#define BUFFER_SIZE 1024

class HttpServer {
public:
    HttpServer(int port) : port_(port) {}

    void start() {
        // Create socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("socket");
            return;
        }

        // Set address and port number for the server
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        // Bind the socket to the address and port
        if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("bind");
            return;
        }

        // Listen for incoming connections
        if (listen(sockfd, 3) < 0) {
            perror("listen");
            return;
        }

        std::cout << "Server listening on port " << port_ << std::endl;

        while (true) {
            // Accept incoming connections
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
            if (client_sockfd < 0) {
                perror("accept");
                continue;
            }

            // Handle client request
            handleRequest(client_sockfd);

            // Close client socket
            close(client_sockfd);
        }
    }

private:
    void handleRequest(int client_sockfd) {
        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            perror("recv");
            return;
        }

        std::string request(buffer, bytes_received);
        std::cout << "Received request: " << request << std::endl;

        // Parse HTTP request
        std::vector<std::string> request_lines;
        size_t pos = 0;
        while ((pos = request.find("\r\n"))!= std::string::npos) {
            request_lines.push_back(request.substr(0, pos));
            request.erase(0, pos + 2);
        }

        // Handle HTTP request
        if (request_lines[0].find("GET")!= std::string::npos) {
            handleGetRequest(client_sockfd);
        } else {
            sendError(client_sockfd, 405, "Method Not Allowed");
        }
    }

    void handleGetRequest(int client_sockfd) {
        // Send HTTP response
        std::string response = "HTTP/1.1 200 OK\r\n\r\nWhatt!";
        send(client_sockfd, response.c_str(), response.size(), 0);
    }

    void sendError(int client_sockfd, int code, const std::string& message) {
        std::string response = "HTTP/1.1 " + std::to_string(code) + " " + message + "\r\n\r\n";
        send(client_sockfd, response.c_str(), response.size(), 0);
    }

    int port_;
};

int main() {
    HttpServer server(PORT);
    server.start();
    return 0;
}
		