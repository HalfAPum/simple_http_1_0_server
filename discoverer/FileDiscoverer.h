//
// Created by o.narvatov on 11/4/2024.
//

#ifndef FILEDISCOVERER_H
#define FILEDISCOVERER_H
#include <string>


class FileDiscoverer {
public:
    static bool findFile(const std::string &uri);
    static std::string getFileContent(const std::string& uri);
};



#endif //FILEDISCOVERER_H
