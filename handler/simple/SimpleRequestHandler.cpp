//
// Created by o.narvatov on 11/4/2024.
//

#include "SimpleRequestHandler.h"

#include "../../discoverer/FileDiscoverer.h"

std::string SimpleRequestHandler::handleRequest(
    const RequestLine &requestLine,
    SimpleResponseStrategy &responseStrategy
) {
    if (requestLine.reqeustURI == "/hello") return "Hi back!";

    if (requestLine.reqeustURI == "/home.html") {
        if (FileDiscoverer::findFile(requestLine.reqeustURI)) {
            return FileDiscoverer::getFileContent(requestLine.reqeustURI);
        }
    }

    return responseStrategy.getErrorResponse(StatusCodes::NOT_FOUND);
}
