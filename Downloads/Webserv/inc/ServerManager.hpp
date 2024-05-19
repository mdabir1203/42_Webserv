#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

#include <vector>
#include "Server.hpp"

class ServerManager
{
    public:
        bool initialize();
        void run();
    private:
        std::vector<Server> servers;
};

#endif