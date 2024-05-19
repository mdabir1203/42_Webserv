#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

class Server {
    public:
        Server(int port);
        bool start();
        int getServerSocket() const;
        int getPort() const;

    private:
        int port;
        int serverSocket;
        struct sockaddr_in serverAddr;


        void createSocket();
        void bindSocket();
        void listenSocket();
};

#endif