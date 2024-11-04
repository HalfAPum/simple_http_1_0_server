//
// Created by o.narvatov on 11/4/2024.
//

#include "RequestHeadersValidator.h"

#include <iostream>

void RequestHeadersValidator::validateAuthorizationHeader(std::stringstream &headerValues) {
    std::string authShema;
    headerValues >> authShema;

    if (headerValues.fail()) return;

    std::string authParam;
    headerValues >> authParam;

    if (headerValues.fail()) return;

    requestHeaders.authScheme = authShema;
    requestHeaders.authParam = authParam;
}

void RequestHeadersValidator::validateContentLengthHeader(std::stringstream &headerValues) {
    headerValues >> requestHeaders.contentLength;

    if (headerValues.fail()) errorCode = StatusCodes::BAD_REQUEST;
}

void RequestHeadersValidator::validateContentTypeHeader(std::stringstream &headerValues) {
    if (headerValues.good()) {
        headerValues >> requestHeaders.contentType;
    } else {
        requestHeaders.contentType = "application/octet-stream";
    }
}

RequestHeaders RequestHeadersValidator::validateHeaders(std::stringstream &request) {
    std::string line;
    while (std::getline(request, line)) {
        //Check for end of headers
        if (line.empty() || line.at(0) == '\r') break;

        //All headers have following structure.
        //header ":" SP content \r

        std::stringstream header(line);

        std::string headerName;
        header >> headerName;

        //Ignore malformed header
        if (headerName.empty() || headerName.at(headerName.size() - 1) != ':') continue;

        //Remove ':' char at the end of the string.
        headerName.pop_back();

        if (headerName == "Authorization") {
            validateAuthorizationHeader(header);
        } else if (headerName == "Content-Encoding") {
            //Cmon I'm not gonna write decoders here.
        } else if (headerName == "Content-Length") {
            validateContentLengthHeader(header);
        } else if (headerName == "Content-Type") {
            validateContentTypeHeader(header);
        } else {
            const std::string headerValue = line.substr(headerName.size() + 2);

            requestHeaders.unprocessedHeaders[headerName] = headerValue;
        }
    }

    return requestHeaders;
}

bool RequestHeadersValidator::areRequestHeadersInvalid() const {
    return errorCode != StatusCodes::NONE;
}
