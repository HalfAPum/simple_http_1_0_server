//
// Created by o.narvatov on 11/4/2024.
//

#ifndef FULLREQUESTHANDLER_H
#define FULLREQUESTHANDLER_H
#include "../../request/FullRequest.h"
#include "../../response/FullResponse.h"


class FullRequestHandler {
public:
    static void handleRequest(const FullRequest &fullRequest, FullResponse &fullResponse);
};



#endif //FULLREQUESTHANDLER_H
