#include <cassert>
#include <iostream>
#include "../src/ConfigParser.cpp"

void testConfigParser()
{
    ConfigParser parser;
    std::vector<ServerConfig> configs;
    assert(parser.parseConfig("config/webserv.conf", configs) == true);
    assert(configs.size() == 1);
    assert(configs[0].port == 8080);
    assert(configs[0].server_name == "example.com");
    assert(configs[0].root == "/var/www/html");
    assert(configs[0].error_page_404 == "/404.html");
    assert(configs[0].location == "/");
    assert(configs[0].proxy_pass == "http://localhost:3000");

    std::cout << "All tests passed!" << std::endl;
}

int main()
{
    testConfigParser();
    return 0;
}
