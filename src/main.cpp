#include "server/HttpServer.hpp"
#include "utils/Logger.hpp"

int main() {
    try {
        HttpServer server(5500);
        server.run();
    } catch (const std::exception& e) {
        Logger::log("Exception: " + std::string(e.what()));
        return 1;
    }
    return 0;
}