#ifndef GEECXX_CURL_H
#define GEECXX_CURL_H

#include <string>
#include <sstream>

namespace geecxx
{

class Curl
{
public:
    static std::string retrievePageTitle(const std::string& url);
    static std::string strHttpError(const long& errorCode);
    static std::string getHttpHeaderField(std::stringstream& headers, const std::string& key);
};

}

#endif //GEECXX_CURL_H
