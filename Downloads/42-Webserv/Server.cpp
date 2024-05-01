#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdio> // Include this to use perror

class Server {
    public:
        Server(int port) : port_(port) {
            sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
            memset(&addr_, 0, sizeof(addr_));
            addr_.sin_family = AF_INET;
            addr_.sin_addr.s_addr = INADDR_ANY;
            addr_.sin_port = htons(port_);

            bind(sockfd_, (struct sockaddr *)&addr_, sizeof(addr_));
            listen(sockfd_, 5);
        }

        ~Server() {
            close(sockfd_);
        }

        void run() {
            while (true) {
                int client = accept(sockfd_, NULL, NULL);
                if (client < 0) {
                    perror("Accept failed");
                    continue;
                }
                handleClient(client);
            }
        }

        void handleClient(int client) {
            // Implement your client handling code here
        }

    private:
        int sockfd_;
        struct sockaddr_in addr_;
        int port_;
};

int main()
{
    Server server(8080); // Create a Server object that listens on port 8080
    std::cout << "Lets goooo moroooonnn !!!";
    server.run();        // Start the server's main loop
    return 0;
}