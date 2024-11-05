//
// Created by o.narvatov on 11/4/2024.
//

#include "FullResponseStrategy.h"

#include <ctime>

const std::string serverHeaderName = "Server";
const std::string serverHeaderValue = "HalfAPumHttpServer/1.0";

const std::string dateHeaderName = "Date";
const std::string contentLengthHeaderName = "Content-Length";
const std::string contentTypeHeaderName = "Content-Type";

const std::string serverHeader = "Server: HalfAPumHttpServer/1.0\r\n";


std::string FullResponseStrategy::getDateHeader() {
    const time_t now = time(nullptr);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));

    return { buffer };
}

//TODO add more server headers in case of error (so send 1 or two headers).
//TODO make it work properly with location header for 3xx responses.
//TODO make it work properly with WWW-Authenticate header for 401 responses.
//TODO configure allow header (need communication with the server impl which types of methods are supported)

std::string FullResponseStrategy::getErrorResponse(const StatusCode errorCode) {
    auto errorResponse = FullResponse();
    errorResponse.setStatusCode(errorCode);

    errorResponse.addHeader(serverHeaderName, serverHeaderValue);
    errorResponse.addHeader(dateHeaderName, getDateHeader());

    //Skip entity body.
    errorResponse.addHeader(contentLengthHeaderName, "0");
    errorResponse.addHeader(contentTypeHeaderName, "text/plain");

    return errorResponse.str();
}

std::string FullResponseStrategy::getFullResponseStr(const std::string &requestMethod, FullResponse &fullResponse) {
    fullResponse.addHeader(serverHeaderName, serverHeaderValue);
    fullResponse.addHeader(dateHeaderName, getDateHeader());

    if (requestMethod == "HEAD") {
        fullResponse.setEntityBody("", "text/plain");
    }

    return fullResponse.str();
}

