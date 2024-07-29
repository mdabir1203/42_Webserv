// Logger.cpp
#include "Logger.hpp"
#include <iostream>
#include <ctime>

void Logger::log(const std::string& message) {
    std::time_t now = std::time(0);
    std::cout << "[" << now << "] " << message << std::endl;
}