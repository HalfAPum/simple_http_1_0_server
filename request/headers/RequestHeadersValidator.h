//
// Created by o.narvatov on 11/4/2024.
//

#ifndef REQUESTHEADERSVALIDATOR_H
#define REQUESTHEADERSVALIDATOR_H
#include <string>

#include "RequestHeaders.h"
#include "sstream"

#include "../../response/StatusCode.h"

//Validates all headers from request: General, Request and Entity
class RequestHeadersValidator {
    RequestHeaders requestHeaders = RequestHeaders();

    void validateAuthorizationHeader(std::stringstream &headerValues);
    void validateContentLengthHeader(std::stringstream &headerValues);
    void validateContentTypeHeader(std::stringstream &headerValues);
public:
    StatusCode errorCode = StatusCodes::NONE;

    RequestHeaders validateHeaders(std::stringstream &request);

    bool areRequestHeadersInvalid() const;
};



#endif //REQUESTHEADERSVALIDATOR_H
