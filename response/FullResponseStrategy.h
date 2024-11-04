//
// Created by o.narvatov on 11/4/2024.
//

#ifndef FULLRESPONSESTRATEGY_H
#define FULLRESPONSESTRATEGY_H
#include "ResponseStrategy.h"


class FullResponseStrategy final : public ResponseStrategy {
public:
    std::string getErrorResponse(StatusCode errorCode) override;

    static std::string getDateHeader();
};



#endif //FULLRESPONSESTRATEGY_H
