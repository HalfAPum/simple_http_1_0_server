//
// Created by o.narvatov on 11/4/2024.
//

#include "SimpleResponseStrategy.h"

std::string SimpleResponseStrategy::getErrorResponse(const StatusCode errorCode) {
    return "Error " + std::to_string(errorCode.statusCode) + ": " + errorCode.reasonPhrase;
}
