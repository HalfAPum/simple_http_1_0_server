//
// Created by o.narvatov on 11/4/2024.
//

#include "FullResponse.h"

#include <iostream>

#include "sstream"

void FullResponse::setStatusCode(const StatusCode &_statusCode) {
    statusCode = _statusCode;
}

void FullResponse::addHeader(const std::string &headerName, const std::string &headerValue) {
    headers[headerName] = headerValue;
}

void FullResponse::setEntityBody(const std::string &_entityBody, const std::string& _contentType) {
    entityBody = _entityBody;
    addHeader("Content-Length", std::to_string(entityBody.size()));
    addHeader("Content-Type", _contentType);
}

std::string FullResponse::str() const {
    std::stringstream ss;
    ss << "HTTP/1.0 " << std::to_string(statusCode.statusCode) << ' ' << statusCode.reasonPhrase << "\r\n";

    for (const auto &[first, second] : headers) {
        ss << first << ": " << second << "\r\n";
    }

    ss << "\r\n";

    if (entityBody.empty()) return ss.str();

    ss << entityBody;

    return ss.str();
}

bool FullResponse::isErrorResponse() const {
    return statusCode.statusCode >= 300;
}
