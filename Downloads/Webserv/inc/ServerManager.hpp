#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

#include <vector>
#include "Server.hpp"

class ServerManager
{
    public:
        bool initialize();
        void run();
        void addServer(const Server& server);
        
    private:
        std::vector<Server> servers;
};

#endif