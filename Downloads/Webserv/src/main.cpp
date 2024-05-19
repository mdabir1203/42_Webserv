#include <iostream>
#include "ServerManager.hpp"

int main()
{
    ServerManager serverM;
    if (!serverM.initialize()) {
        std::cerr << "Error initializing server" << std::endl;
        return (true);
    }
    serverM.run();
    return (0);
}