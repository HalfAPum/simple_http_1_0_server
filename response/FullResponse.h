//
// Created by o.narvatov on 11/4/2024.
//

#ifndef FULLRESPONSE_H
#define FULLRESPONSE_H
#include <string>
#include <unordered_map>

#include "StatusCode.h"


class FullResponse {
    StatusCode statusCode = StatusCodes::OK;
    std::unordered_map<std::string, std::string> headers {};
    std::string entityBody;
public:
    void setStatusCode(const StatusCode &_statusCode);
    void addHeader(const std::string &headerName, const std::string &headerValue);
    void setEntityBody(const std::string &_entityBody, const std::string& _contentType);

    std::string str() const;

    bool isErrorResponse() const;

    StatusCode getErrorCode() const { return statusCode; }
};



#endif //FULLRESPONSE_H
