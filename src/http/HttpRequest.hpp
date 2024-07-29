#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include <vector>

class HttpRequest {
public:
    HttpRequest();
    void parse(const std::string& raw_request);

    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getVersion() const;
    const std::map<std::string, std::string>& getHeaders() const;

private:
    enum class ParserState {
        METHOD,
        PATH,
        VERSION,
        HEADER_NAME,
        HEADER_VALUE,
        BODY,
        DONE
    };

    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    void parseChar(char c);
    void transitionState(ParserState new_state);
    void finishCurrentToken();

    ParserState current_state;
    std::string current_token;
    std::string current_header_name;
};

#endif // HTTP_REQUEST_HPP