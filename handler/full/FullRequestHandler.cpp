//
// Created by o.narvatov on 11/4/2024.
//

#include "FullRequestHandler.h"

#include <iostream>

#include "../../discoverer/FileDiscoverer.h"

void FullRequestHandler::handleRequest(const FullRequest &fullRequest, FullResponse &fullResponse) {
    if (fullRequest.requestLine.reqeustURI == "/hello") {
        fullResponse.setStatusCode(StatusCodes::OK);
        fullResponse.setEntityBody("Hi back! from HTTP/1.0", "text/plain");
        return;
    }

    if (fullRequest.requestLine.reqeustURI == "/home.html") {
        if (FileDiscoverer::findFile(fullRequest.requestLine.reqeustURI)) {
            if (fullRequest.requestLine.method == "GET") {
                fullResponse.setStatusCode(StatusCodes::OK);
                fullResponse.setEntityBody(
                    FileDiscoverer::getFileContent(fullRequest.requestLine.reqeustURI),
                    "text/html"
                );
                return;
            }

            if (fullRequest.requestLine.method == "HEAD") {
                fullResponse.setStatusCode(StatusCodes::OK);
                return;
            }

            fullResponse.setStatusCode(StatusCodes::BAD_REQUEST);
            fullResponse.addHeader("Allow", "GET, HEAD");
            return;
        }
    }

    fullResponse.setStatusCode(StatusCodes::NOT_FOUND);
}
