//
// Created by o.narvatov on 11/4/2024.
//

#include "iostream"
#include "sstream"

#include "RequestLineValidator.h"
#include "../../response/StatusCode.h"

RequestLine RequestLineValidator::validateRequestLine(const std::string &line) {
    std::stringstream ssLine(line);

    std::string method;
    ssLine >> method;

    if (ssLine.fail()) {
        errorCode = StatusCodes::BAD_REQUEST;
        return processedRequestLine;
    }

    if (!validateHttpMethod(method)) {
        errorCode = StatusCodes::NOT_IMPLEMENTED;
        return processedRequestLine;
    }

    processedRequestLine.method = method;

    std::string requestURI;
    ssLine >> requestURI;

    if (ssLine.fail()) {
        errorCode = StatusCodes::BAD_REQUEST;
        return processedRequestLine;
    }

    if (!validateRequestURI(requestURI)) {
        errorCode = StatusCodes::BAD_REQUEST;
        return processedRequestLine;
    }

    processedRequestLine.reqeustURI = requestURI;

    std::string httpVersion;
    ssLine >> httpVersion;

    //No HTTP version specified. We have HTTP version 0.9.
    if (ssLine.fail()) {
        processedRequestLine.httpMajorVersion = 0;
        processedRequestLine.httpMinorVersion = 9;
    }

    if (!validateHttpVersion(httpVersion)) {
        errorCode = StatusCodes::BAD_REQUEST;
        return processedRequestLine;
    }

    return processedRequestLine;
}

bool RequestLineValidator::validateHttpMethod(const std::string &method) {
    if (method == "GET" || method == "POST" || method == "HEAD") {
        return true;
    } else {
        //TODO ASK SERVER IF IT SUPPORTS THIS METHOD (method also must be in format of token)
        return false;
    }
}

bool RequestLineValidator::validateRequestURI(const std::string &requestURI) {
    // Server can only receive requestURI as abs path. abs_path specification:
    // abs_path       = "/" rel_path
    // rel_path       = [ path ] [ ";" params ] [ "?" query ]
    // path           = fsegment *( "/" segment )
    // fsegment       = 1*pchar
    // segment        = *pchar
    //
    // Implement only validation of path part for my sanity).
    // Assume requests don't have params and query.
    // Don't check for ; even in path since we don't validate params.

    if (requestURI.empty() || requestURI.at(0) != '/') return false;

    for (const char &c : requestURI) {
        const int char_ascii_index = c;

        //Exclude CTL:
        if (char_ascii_index <= 31 || char_ascii_index == 127) {
            return false;
        }

        switch (c) {
            case ' ':
            case '"':
            case '#':
            case '%':
            case '<':
            case '>':
                return false;
            default:
                return true;
        }
    }

    //Unreachable. We always have non-empty string before the loop.
    return false;
}


bool isNumber(const std::string& s){
    auto it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool RequestLineValidator::validateHttpVersion(const std::string &httpVersion) {
    if (httpVersion.rfind("HTTP/", 0) == std::string::npos) return false;

    const auto index = httpVersion.find('.');

    if (index == std::string::npos) return false;

    const std::string httpMajorVersion = httpVersion.substr(5, index - 5);

    if (!isNumber(httpMajorVersion)) return false;

    if (index + 1 > httpVersion.size()) return false;

    const std::string httpMinorVersion = httpVersion.substr(index + 1, httpVersion.size() - (index + 1));

    if (!isNumber(httpMinorVersion)) return false;

    processedRequestLine.httpMajorVersion = stoi(httpMajorVersion);
    processedRequestLine.httpMinorVersion = stoi(httpMinorVersion);

    return true;
}

bool RequestLineValidator::isRequestLineInvalid() const {
    return errorCode != StatusCodes::NONE;
}
