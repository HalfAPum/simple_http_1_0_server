//
// Created by o.narvatov on 11/4/2024.
//

#ifndef STATUSCODE_H
#define STATUSCODE_H
#include <string>

struct StatusCode {
    int statusCode;
    std::string reasonPhrase;

    bool operator==(const StatusCode &other) const;
    bool operator!=(const StatusCode &other) const;
};

inline bool StatusCode::operator==(const StatusCode &other) const {
    return statusCode == other.statusCode;
}

inline bool StatusCode::operator!=(const StatusCode &other) const {
    return !(*this == other);
}

namespace StatusCodes {
    const StatusCode NONE = { -1, "UNKNOWN" };

    const StatusCode OK = { 200, "OK" };
    const StatusCode CREATED = { 201, "Created" };
    const StatusCode ACCEPTED = { 202, "Accepted" };
    const StatusCode NO_CONTENT = { 204, "No Content" };

    const StatusCode MULTIPLE_CHOICES = { 300, "Multiple Choices" };
    const StatusCode MOVED_PERMANENTLY = { 301, "Moved Permanently" };
    const StatusCode MOVED_TEMPORARILY = { 302, "Moved Temporarily" };
    const StatusCode NOT_MODIFIED = { 304, "Not Modified" };

    const StatusCode BAD_REQUEST = { 400, "Bad Request" };
    const StatusCode UNAUTHORIZED = { 401, "Unauthorized" };
    const StatusCode FORBIDDEN = { 403, "Forbidden" };
    const StatusCode NOT_FOUND = { 404, "Not Found" };

    const StatusCode INTERNAL_SERVER_ERROR = { 500, "Internal Server Error" };
    const StatusCode NOT_IMPLEMENTED = { 501, "Not Implemented" };
    const StatusCode BAD_GATEWAY = { 502, "Bad Gateway" };
    const StatusCode SERVICE_UNAVAILABLE = { 503, "Service Unavailable" };
}

#endif //STATUSCODE_H
