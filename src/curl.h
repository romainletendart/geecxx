#ifndef GEECXX_CURL_H
#define GEECXX_CURL_H

#include <string>

namespace geecxx
{

class Curl
{
public:
    static std::string retrievePageTitle(const std::string& url);
};

}

#endif //GEECXX_CURL_H
