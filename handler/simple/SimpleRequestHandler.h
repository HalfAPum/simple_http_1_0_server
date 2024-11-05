//
// Created by o.narvatov on 11/4/2024.
//

#ifndef SIMPLEREQUESTHANDLER_H
#define SIMPLEREQUESTHANDLER_H
#include <string>

#include "../../request/line/RequestLine.h"
#include "../../response/SimpleResponseStrategy.h"


//HTTP 0.9 requests handler
class SimpleRequestHandler {
public:
    static std::string handleRequest(const RequestLine &requestLine, SimpleResponseStrategy &responseStrategy);
};



#endif //SIMPLEREQUESTHANDLER_H
