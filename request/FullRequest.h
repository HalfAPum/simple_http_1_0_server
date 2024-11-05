//
// Created by o.narvatov on 11/4/2024.
//

#ifndef FULLREQUEST_H
#define FULLREQUEST_H
#include "headers/RequestHeaders.h"
#include "line/RequestLine.h"

struct FullRequest {
    RequestLine requestLine;
    RequestHeaders requestHeaders;
    std::string requestBody;
};

#endif //FULLREQUEST_H
