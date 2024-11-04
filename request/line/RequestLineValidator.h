//
// Created by o.narvatov on 11/4/2024.
//

#ifndef REQUESTLINEVALIDATOR_H
#define REQUESTLINEVALIDATOR_H
#include <string>

#include "RequestLine.h"
#include "../../response/StatusCode.h"


class RequestLineValidator {
    RequestLine processedRequestLine = RequestLine();

    bool validateHttpMethod(const std::string &method);
    bool validateRequestURI(const std::string &requestURI);
    bool validateHttpVersion(const std::string &httpVersion);
public:
    StatusCode errorCode = StatusCodes::NONE;

    RequestLine validateRequestLine(const std::string &line);

    bool isRequestLineInvalid() const;
};



#endif //REQUESTLINEVALIDATOR_H
