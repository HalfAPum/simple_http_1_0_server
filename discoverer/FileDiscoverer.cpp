//
// Created by o.narvatov on 11/4/2024.
//

#include "FileDiscoverer.h"

#include <fstream>
#include <sstream>

const std::string fullPathStart = "C:/Users/o.narvatov/CLionProjects/simple-http-1.0-server/";

bool FileDiscoverer::findFile(const std::string &uri) {
    std::ifstream file(fullPathStart + uri);

    const bool result = file.is_open();

    file.close();

    return result;
}

std::string FileDiscoverer::getFileContent(const std::string& uri) {
    std::ifstream file(fullPathStart + uri);

    std::stringstream ss;
    ss << file.rdbuf();

    file.close();

    return ss.str();
}

