//
// Created by o.narvatov on 11/4/2024.
//

#ifndef SIMPLERESPONSESTRATEGY_H
#define SIMPLERESPONSESTRATEGY_H

#include "ResponseStrategy.h"



class SimpleResponseStrategy final : public ResponseStrategy {
public:
    std::string getErrorResponse(StatusCode errorCode) override;
};



#endif //SIMPLERESPONSESTRATEGY_H
