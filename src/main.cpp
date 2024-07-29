#include <iostream>
#include "server.hpp"

int main()
{
    HTTPServer server;

    if (!server.initialize())
    {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }

    std::cout << "Server started.. Running..." << std::endl;
    server.run();
    return 0;
}