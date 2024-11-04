//
// Created by o.narvatov on 11/4/2024.
//

#ifndef REQUESTHEADERS_H
#define REQUESTHEADERS_H
#include <string>
#include <unordered_map>

struct RequestHeaders {
    //Authorization
    std::string authScheme;
    std::string authParam;

    //Content
    unsigned contentLength;
    std::string contentType;

    std::unordered_map<std::string, std::string> unprocessedHeaders;
};

#endif //REQUESTHEADERS_H
