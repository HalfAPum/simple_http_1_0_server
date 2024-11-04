//
// Created by o.narvatov on 11/4/2024.
//

#ifndef SENDSTRATEGY_H
#define SENDSTRATEGY_H
#include "StatusCode.h"

class ResponseStrategy {
public:
    virtual std::string getErrorResponse(StatusCode errorCode);
};

#endif //SENDSTRATEGY_H
