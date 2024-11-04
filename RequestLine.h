//
// Created by o.narvatov on 11/3/2024.
//

#ifndef REQUESTLINE_H
#define REQUESTLINE_H
#include <string>

struct RequestLine {
    std::string method;
    std::string reqeustURI;
    unsigned httpMajorVersion;
    unsigned httpMinorVersion;
};

#endif //REQUESTLINE_H
