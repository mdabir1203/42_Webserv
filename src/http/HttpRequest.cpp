// HttpRequest.cpp
#include "HttpRequest.hpp"
#include <stdexcept>

HttpRequest::HttpRequest() : current_state(ParserState::METHOD) {}

void HttpRequest::parse(const std::string& raw_request) {
    for (char c : raw_request) {
        parseChar(c);
    }
    finishCurrentToken();
    if (current_state != ParserState::DONE && current_state != ParserState::BODY) {
        throw std::runtime_error("Incomplete HTTP request");
    }
}

void HttpRequest::parseChar(char c) {
    switch (current_state) {
        case ParserState::METHOD:
            if (c == ' ') {
                finishCurrentToken();
                transitionState(ParserState::PATH);
            } else {
                current_token += c;
            }
            break;

        case ParserState::PATH:
            if (c == ' ') {
                finishCurrentToken();
                transitionState(ParserState::VERSION);
            } else {
                current_token += c;
            }
            break;

        case ParserState::VERSION:
            if (c == '\r') {
                // Ignore
            } else if (c == '\n') {
                finishCurrentToken();
                transitionState(ParserState::HEADER_NAME);
            } else {
                current_token += c;
            }
            break;

        case ParserState::HEADER_NAME:
            if (c == ':') {
                finishCurrentToken();
                transitionState(ParserState::HEADER_VALUE);
            } else if (c == '\r') {
                // Ignore
            } else if (c == '\n' && current_token.empty()) {
                transitionState(ParserState::BODY);
            } else {
                current_token += c;
            }
            break;

        case ParserState::HEADER_VALUE:
            if (c == '\r') {
                // Ignore
            } else if (c == '\n') {
                finishCurrentToken();
                transitionState(ParserState::HEADER_NAME);
            } else if (!current_token.empty() || c != ' ') {
                current_token += c;
            }
            break;

        case ParserState::BODY:
            body += c;
            break;

        case ParserState::DONE:
            // Ignore any additional characters
            break;
    }
}

void HttpRequest::transitionState(ParserState new_state) {
    current_state = new_state;
    current_token.clear();
}

void HttpRequest::finishCurrentToken() {
    switch (current_state) {
        case ParserState::METHOD:
            method = current_token;
            break;
        case ParserState::PATH:
            path = current_token;
            break;
        case ParserState::VERSION:
            version = current_token;
            break;
        case ParserState::HEADER_NAME:
            current_header_name = current_token;
            break;
        case ParserState::HEADER_VALUE:
            headers[current_header_name] = current_token;
            break;
        default:
            // Do nothing for other states
            break;
    }
    current_token.clear();
}

const std::string& HttpRequest::getMethod() const { return method; }
const std::string& HttpRequest::getPath() const { return path; }
const std::string& HttpRequest::getVersion() const { return version; }
const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return headers; }