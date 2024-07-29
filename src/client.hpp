// client.hpp
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
    int socket_fd;
    std::string buffer;

public:
    Client(int sock_fd);
    ~Client();

    bool readData();
    bool writeData(const std::string& response);
    bool handleRequest();
    void close();

    int getSocket() const { return socket_fd; }

private:
    std::string processRequest(const std::string& request);
};

#endif // CLIENT_HPP