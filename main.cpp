#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>

#include "RequestLine.h"
#include "StatusCode.h"

#define DEFAULT_PORT "8080"
#define DEFAULT_BUFLEN 512

char recvbuf[DEFAULT_BUFLEN];
int iResult, iSendResult;
int recvbuflen = DEFAULT_BUFLEN;

addrinfo *result = nullptr, *ptr = nullptr, hints;

StatusCode errorCode;

boolean validateHttpMethod(const std::string &method) {
    if (method == "GET" || method == "POST" || method == "HEAD") {
        return true;
    } else {
        //TODO ASK SERVER IF IT SUPPORTS THIS METHOD (method also must be in format of token)
        return false;
    }
}

boolean validateRequestURI(const std::string &requestURI) {
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

boolean validateHttpVersion(const std::string &httpVersion, RequestLine &requestLine) {
    if (httpVersion.rfind("HTTP/", 0) == std::string::npos) return false;

    const auto index = httpVersion.find('.');

    if (index == std::string::npos) return false;

    const std::string httpMajorVersion = httpVersion.substr(5, index - 5);

    if (!isNumber(httpMajorVersion)) return false;

    if (index + 1 > httpVersion.size()) return false;

    const std::string httpMinorVersion = httpVersion.substr(index + 1, httpVersion.size() - (index + 1));

    if (!isNumber(httpMinorVersion)) return false;

    requestLine.httpMajorVersion = stoi(httpMajorVersion);
    requestLine.httpMinorVersion = stoi(httpMinorVersion);

    return true;
}

boolean validateRequestLine(const std::string &line, RequestLine &requestLine) {
    std::stringstream ssLine(line);

    std::string method;
    ssLine >> method;

    if (ssLine.fail()) {
        errorCode = StatusCodes::BAD_REQUEST;
        return false;
    }

    if (!validateHttpMethod(method)) {
        errorCode = StatusCodes::NOT_IMPLEMENTED;
        return false;
    }

    requestLine.method = method;

    std::string requestURI;
    ssLine >> requestURI;

    if (ssLine.fail()) {
        errorCode = StatusCodes::BAD_REQUEST;
        return false;
    }

    if (!validateRequestURI(requestURI)) {
        errorCode = StatusCodes::BAD_REQUEST;
        return false;
    }

    requestLine.reqeustURI = requestURI;

    std::string httpVersion;
    ssLine >> httpVersion;

    //No HTTP version specified. We have HTTP version 0.9.
    if (ssLine.fail()) {
        requestLine.httpMajorVersion = 0;
        requestLine.httpMinorVersion = 9;
    }

    if (!validateHttpVersion(httpVersion, requestLine)) {
        //TODO Server should return the status code 400 (bad request)
        return false;
    }

    std::cout << "method: " << requestLine.method << std::endl;
    std::cout << "request_URI: " << requestLine.reqeustURI << std::endl;
    std::cout << "httpVersion: " << "HTTP/" << requestLine.httpMajorVersion << '.' << requestLine.httpMinorVersion << std::endl;
    std::cout << "is fail?: " << ssLine.fail() << std::endl;

    return true;
}


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

    // Receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            //1. Read request line HTTP 0.9 request doesn't have HTTP-Version in request-line
            // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
            std::stringstream request(recvbuf);
            std::string line;

            std::getline(request, line);

            RequestLine requestLine;

            if (!validateRequestLine(line, requestLine)) {
                //send error reponse
                //do shutdown (don't copy paste)
            }

            //old
            // Echo the buffer back to the sender
            iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

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
