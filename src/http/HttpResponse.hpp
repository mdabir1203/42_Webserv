#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
    public:
        HttpResponse();
        void setStatus(int code, const std::string& message);
        void addHeader(const std::string& key, const std::string& value);
        void setBody(const std::string& body);
        std::string toString() const;

    private:
        std::string version;
        int status_code;
        std::string status_message;
        std::map<std::string, std::string> headers;
        std::string body;
};

#endif