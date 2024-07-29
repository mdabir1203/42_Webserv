#include "client.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

Client::Client(int sock_fd) : socket_fd(sock_fd), buffer("") {}

Client::~Client() {
    close();
}


bool Client::readData() {
    char buf[1024];
    ssize_t bytes_read = recv(socket_fd, buf, sizeof(buf), 0);
    if (bytes_read > 0) {
        buffer.append(buf, bytes_read);
        return true;
    } else if (bytes_read == 0) {
        // Connection closed by client
        return false;
    } else {
        // Error occurred
        std::cerr << "Error reading from client socket" << std::endl;
        return false;
    }
}

bool Client::writeData(const std::string& response) {
    ssize_t bytes_sent = send(socket_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << "Error sending response to client" << std::endl;
        return false;
    }
    return true;
}

bool Client::handleRequest() {
    if (!readData()) {
        return false;
    }

    size_t pos = buffer.find("\r\n\r\n");
    if (pos == std::string::npos) {
        // Incomplete request, continue reading
        return true;
    }

    std::string request = buffer.substr(0, pos);
    buffer.erase(0, pos + 4);

    std::string response = processRequest(request);
    return writeData(response);
}

void Client::close() {
    if (socket_fd != -1) {
        ::close(socket_fd);
        socket_fd = -1;
    }
}

std::string Client::processRequest(const std::string& request) {
    // Simple HTTP response
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\r\n"
       << "Content-Type: text/plain\r\n"
       << "Connection: close\r\n"
       << "\r\n"
       << "Welcome to crazyCrow Webserver!";
    return ss.str();
}