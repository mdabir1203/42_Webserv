// test_HttpRequest.cpp
#include <gtest/gtest.h>
#include "../src/http/HttpRequest.hpp"

TEST(HttpRequestTest, ParsesValidRequest) {
    std::string raw_request =
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "\r\n";

    HttpRequest request;
    request.parse(raw_request);

    EXPECT_EQ(request.getMethod(), "GET");
    EXPECT_EQ(request.getPath(), "/index.html");
    EXPECT_EQ(request.getVersion(), "HTTP/1.1");
    EXPECT_EQ(request.getHeaders().at("Host"), "example.com");
    EXPECT_EQ(request.getHeaders().at("User-Agent"), "Mozilla/5.0");
}

TEST(HttpRequestTest, HandlesRequestWithBody) {
    std::string raw_request =
        "POST /submit HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "key1=value1&key2=value2";

    HttpRequest request;
    request.parse(raw_request);

    EXPECT_EQ(request.getMethod(), "POST");
    EXPECT_EQ(request.getPath(), "/submit");
    EXPECT_EQ(request.getVersion(), "HTTP/1.1");
    EXPECT_EQ(request.getHeaders().at("Content-Type"), "application/x-www-form-urlencoded");
    EXPECT_EQ(request.getHeaders().at("Content-Length"), "13");
    // You might want to add a getBody() method to HttpRequest to test the body content
}

TEST(HttpRequestTest, ThrowsOnIncompleteRequest) {
    std::string raw_request =
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0\r\n";

    HttpRequest request;
    EXPECT_THROW(request.parse(raw_request), std::runtime_error);
}