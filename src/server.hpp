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


class HttpServer {
    public: 
        HttpServer(const std::vector<int>&ports, const std::map<std::string, std::string>&hostnames);
        void start();

    private:

        struct RouteInfo {
            std::string directory;
            std::vector<std::string> allowedMethods;
        };

        void startServer(int port);
        void handleRequest(int client_sockfd);
        void handleGetRequest(int client_sockfd, const std::string& path);
        void handlePostRequest(int client_sockfd, const std::string& request);
        void sendError(int client_sockfd, int code, const std::string& message);
        void addRoute(const std::string& path, const RouteInfo& routeInfo);
        
        std::vector<int> ports_;
        std::map<std::string, std::string> hostnames_;
        std::map<std::string, RouteInfo> routes_;

            
        static const int BUFFER_SIZE = 1024;
        static const int MAX_BODY_SIZE = 1048576; // 1MB
        static const char* DEFAULT_FILE;
};