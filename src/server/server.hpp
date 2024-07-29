#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/select.h>
#include <map>
#include <vector>

class Client {
public:
    Client(int socket);
    bool handleRequest();
    int getSocket() const;
private:
    int client_socket;
};

class HTTPServer {
public:
    HTTPServer();
    ~HTTPServer();
    bool initialize();
    void run();
private:
    void acceptNewClient();
    void handleClients();
    int selectCall(fd_set& read_fds);
    void closeSocket(int socket);

    int server_socket;
    std::map<int, Client*> clients;
};

#endif // SERVER_HPP