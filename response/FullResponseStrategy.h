//
// Created by o.narvatov on 11/4/2024.
//

#ifndef FULLRESPONSESTRATEGY_H
#define FULLRESPONSESTRATEGY_H
#include "ResponseStrategy.h"
#include "FullResponse.h"


class FullResponseStrategy final : public ResponseStrategy {
public:
    std::string getErrorResponse(StatusCode errorCode) override;

    static std::string getDateHeader();

    static std::string getFullResponseStr(const std::string& requestMethod, FullResponse &fullResponse);
};



#endif //FULLRESPONSESTRATEGY_H
