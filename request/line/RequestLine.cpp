//
// Created by o.narvatov on 11/4/2024.
//

#include "RequestLine.h"

bool RequestLine::isSimpleRequest() const {
    return httpMajorVersion == 0 && httpMinorVersion == 0;
}

bool RequestLine::isFileURI() const {
    return reqeustURI.find('.') != std::string::npos;
}
