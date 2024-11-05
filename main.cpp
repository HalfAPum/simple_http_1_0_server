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

addrinfo *result = nullptr, *ptr = nullptr, hints;

auto simpleResponseStrategy = SimpleResponseStrategy();
auto fullResponseStrategy = FullResponseStrategy();

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;


bool checkResultFail(const bool result, const std::string &actionName, const SOCKET socket) {
    if (!result) return false;

    std::cout << actionName << " failed with error: " << WSAGetLastError() << std::endl;

    closesocket(socket);
    WSACleanup();

    return true;
}

bool setUpTCPConnection() {
    WSADATA wsaData = {};

    addrinfo *result = nullptr;
    addrinfo hints{};

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (checkResultFail(iResult == SOCKET_ERROR, "bind", ListenSocket)) {
        freeaddrinfo(result);
        return false;
    }

    freeaddrinfo(result);

    return true;
}

void printRequest(char buffer[DEFAULT_BUFLEN], int size) {
    for(int i = 0 ; i < size ; ++i){
        std::cout << buffer[i];
    }
    std::cout << std::endl;
    printf("\n");
}

void shutDownSocket(const SOCKET socket) {
    // shutdown the connection since we're done
    const auto shutdownResult = shutdown(socket, SD_SEND);

    if (checkResultFail(shutdownResult == SOCKET_ERROR, "shutdown", socket)) return;

    // cleanup
    closesocket(socket);
    WSACleanup();
}

void sendMessage(const SOCKET socket, const std::string &message) {
    const auto sendResult = send(socket, message.c_str(), message.length(), 0);

    if (checkResultFail(sendResult == SOCKET_ERROR, "send", socket)) return;

    printf("Bytes sent: %d\n", sendResult);

    shutDownSocket(ClientSocket);
}

int main() {
    if (!setUpTCPConnection()) return 1;

    auto iResult = listen(ListenSocket, SOMAXCONN);
    if (checkResultFail(iResult == SOCKET_ERROR, "listen", ListenSocket)) return 1;

    // Accept a client socket
    ClientSocket = accept(ListenSocket, nullptr, nullptr);
    if (checkResultFail(ClientSocket == INVALID_SOCKET, "accept", ListenSocket)) return 1;

    // No longer need server socket
    closesocket(ListenSocket);

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

    if (checkResultFail(iResult <= 0, "recv", ClientSocket)) return 1;

    printf("Bytes received: %d\n", iResult);

    printRequest(recvbuf, iResult);

    std::stringstream request(recvbuf);
    std::string requestLine;

    std::getline(request, requestLine);


    auto requestLineValidator = RequestLineValidator();
    RequestLine processedRequestLine = requestLineValidator.validateRequestLine(requestLine);

    ResponseStrategy *responseStrategy;
    if (processedRequestLine.isSimpleRequest()) {
        responseStrategy = &simpleResponseStrategy;
    } else {
        responseStrategy = &fullResponseStrategy;
    }

    if (requestLineValidator.isRequestLineInvalid()) {
        const std::string errorResponse = responseStrategy->getErrorResponse(requestLineValidator.errorCode);
        sendMessage(ClientSocket, errorResponse);
        return 0;
    }

    if (processedRequestLine.isSimpleRequest()) {
        const auto response = SimpleRequestHandler::handleRequest(processedRequestLine, simpleResponseStrategy);
        sendMessage(ClientSocket, response);
        return 0;
    }

    //Validate headers
    auto requestHeadersValidator = RequestHeadersValidator();
    RequestHeaders requestHeaders = requestHeadersValidator.validateHeaders(request);

    if (requestHeadersValidator.areRequestHeadersInvalid()) {
        const auto response = responseStrategy->getErrorResponse(requestHeadersValidator.errorCode);
        sendMessage(ClientSocket, response);
        return 0;
    }

    std::string requestBody(request.str());

    FullRequest fullRequest { processedRequestLine, requestHeaders, requestBody };

    auto fullResponse = FullResponse();

    FullRequestHandler::handleRequest(fullRequest, fullResponse);

    std::string response;
    if (fullResponse.isErrorResponse()) {
        response = responseStrategy->getErrorResponse(fullResponse.getErrorCode());
    } else {
        response = FullResponseStrategy::getFullResponseStr(processedRequestLine.method, fullResponse);\
    }

    sendMessage(ClientSocket, response);

    return 0;
}
