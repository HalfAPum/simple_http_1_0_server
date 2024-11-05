#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>

#include "handler/full/FullRequestHandler.h"
#include "handler/simple/SimpleRequestHandler.h"
#include "request/FullRequest.h"
#include "request/headers/RequestHeadersValidator.h"
#include "request/line/RequestLine.h"
#include "request/line/RequestLineValidator.h"
#include "response/FullResponse.h"
#include "response/FullResponseStrategy.h"
#include "response/ResponseStrategy.h"
#include "response/SimpleResponseStrategy.h"

#define DEFAULT_PORT "8080"
#define DEFAULT_BUFLEN 512

char recvbuf[DEFAULT_BUFLEN];
int iResult, iSendResult;
int recvbuflen = DEFAULT_BUFLEN;

addrinfo *result = nullptr, *ptr = nullptr, hints;

auto simpleResponseStrategy = SimpleResponseStrategy();
auto fullResponseStrategy = FullResponseStrategy();


int main()
{
    WSADATA wsaData = {};

    addrinfo *result = nullptr;
    addrinfo hints{};

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    SOCKET ClientSocket = accept(ListenSocket, nullptr, nullptr);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);

        for(int i=0 ; i<recvbuflen ; ++i){
            std::cout << recvbuf[i];
        }
        std::cout << std::endl;
        printf("\n");

        //1. Read request line HTTP 0.9 request doesn't have HTTP-Version in request-line
        // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
        std::stringstream request(recvbuf);
        std::string line;

        std::getline(request, line);


        auto requestLineValidator = RequestLineValidator();
        RequestLine processedRequestLine = requestLineValidator.validateRequestLine(line);

        ResponseStrategy *responseStrategy;
        if (processedRequestLine.isSimpleRequest()) {
            responseStrategy = &simpleResponseStrategy;
        } else {
            responseStrategy = &fullResponseStrategy;
        }

        if (requestLineValidator.isRequestLineInvalid()) {
            std::string errorResponse = responseStrategy->getErrorResponse(requestLineValidator.errorCode);

            iSendResult = send(ClientSocket, errorResponse.c_str(), errorResponse.length(), 0);
            //do shutdown (don't copy paste)
        } else {
            std::string response;

            if (processedRequestLine.isSimpleRequest()) {
                response = SimpleRequestHandler::handleRequest(processedRequestLine, simpleResponseStrategy);
            } else {
                //Validate headers
                auto requestHeadersValidator = RequestHeadersValidator();
                RequestHeaders requestHeaders = requestHeadersValidator.validateHeaders(request);

                if (requestHeadersValidator.areRequestHeadersInvalid()) {
                    response = responseStrategy->getErrorResponse(requestHeadersValidator.errorCode);
                } else {
                    std::string requestBody(request.str());

                    FullRequest fullRequest { processedRequestLine, requestHeaders, requestBody };

                    auto fullResponse = FullResponse();

                    FullRequestHandler::handleRequest(fullRequest, fullResponse);

                    if (fullResponse.isErrorResponse()) {
                        response = responseStrategy->getErrorResponse(fullResponse.getErrorCode());
                    } else {
                        response = FullResponseStrategy::getFullResponseStr(processedRequestLine.method, fullResponse);\
                    }
                }
            }

            iSendResult = send(ClientSocket, response.c_str(), response.length(), 0);
        }
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        printf("Bytes sent: %d\n", iSendResult);
    } else  {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
