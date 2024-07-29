#include "HttpResponse.hpp"
#include <sstream>

HttpResponse::HttpResponse() : version("HTTP/1.1"), status_code(200), status_message("OK") {}

void HttpResponse::setStatus(int code, const std::string& message) {
    status_code = code;
    status_message = message;
}

void HttpResponse::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void HttpResponse::setBody(const std::string& new_body)
{
    body = new_body;
}

// For clear identification iterator is declared
std::string HttpResponse::toString() const {
    std::ostringstream response;
    response << version << " " << status_code << " " << status_message << "\r\n";

    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n" << body;
    return response.str();
}