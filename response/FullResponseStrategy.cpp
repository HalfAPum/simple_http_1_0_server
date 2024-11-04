//
// Created by o.narvatov on 11/4/2024.
//

#include "FullResponseStrategy.h"

#include <ctime>

const std::string serverHeader = "Server: HalfAPumHttpServer/1.0\r\n";

std::string FullResponseStrategy::getDateHeader() {
    const time_t now = time(nullptr);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));

    return "Date: " + std::string(buffer) + "\r\n";
}

std::string FullResponseStrategy::getErrorResponse(const StatusCode errorCode) {
    //We support only HTTP 1.0 so the only Full-Response possible is 1.0 version.
    const std::string statusLine = "HTTP/1.0 " + std::to_string(errorCode.statusCode) + ' ' + errorCode.reasonPhrase + "\r ";

    //TODO add more server headers in case of error (so send 1 or two headers).
    //TODO make it work properly with location header for 3xx responses.
    //TODO make it work properly with WWW-Authenticate header for 401 responses.

    const std::string generalHeader = getDateHeader();
    const std::string responseHeader = serverHeader;

    //TODO configure allow header (need communication with the server impl which types of methods are supported)
    //Skip entity body.
    const std::string contentLength = "Content-Length: 0\r\n";
    const std::string contentType = "Content-Type: text/plain\r\n";
    const std::string entityHeader = contentLength + contentType;

    return statusLine + generalHeader + responseHeader + entityHeader + "\r\n";
}
