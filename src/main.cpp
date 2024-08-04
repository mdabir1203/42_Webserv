#include "server.hpp"


int main() {
    std::vector<int> ports;
    ports.push_back(8800);
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