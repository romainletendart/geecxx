#include <sstream>
#include <string>
#include <curl/curl.h>
#include "curl.h"
#include "logger.h"

namespace
{

static size_t curlWriteCallBack(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::stringstream*)userp)->write((char*)contents, size * nmemb);
    return size * nmemb;
}

}

namespace geecxx
{

Curl::~Curl()
{
    curl_global_cleanup();
}

Curl& Curl::getInstance()
{
    // Static variable initialization ensures threadsafety in C++11 standard
    static Curl curl;
    return curl;
}

std::string Curl::retrievePageTitle(const std::string& url)
{
    if (!_isInitiliazed) {
        return "Unable to retrieve page title (please check the logs).";
    }

    CURL *curl;
    CURLcode res;
    std::stringstream readBuffer;
    std::string title = "Untitled";
    long errorCode;

    //Retrieving headers information first
    curl = curl_easy_init();
    if (!curl) {
       LOG_ERROR("Failed to allocate curl object.");
       return title;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ::curlWriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &errorCode);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        return std::string(curl_easy_strerror(res));
    }
    if (errorCode != 200l) {
        return strHttpError(errorCode);
    }
    std::string contentType = getHttpHeaderField(readBuffer, "Content-Type");
    if (contentType.find("text/html") == std::string::npos) {
        return title;
    }
    
    //We can now retrieve the html
    curl = curl_easy_init();
    if (!curl) {
       LOG_ERROR("Failed to allocate curl object.");
       return title;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ::curlWriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &errorCode);
    curl_easy_cleanup(curl);
    if(res != CURLE_OK) {
        return std::string(curl_easy_strerror(res));
    }
    if (errorCode != 200l) {
        return strHttpError(errorCode);
    }

    //parsing title
    size_t pos = readBuffer.str().find("<title>") + 7;
    size_t pos2 = readBuffer.str().find("</title>");

    if (pos != std::string::npos && pos2 != std::string::npos) {
        title = readBuffer.str().substr(pos, pos2 - pos);
    }
    return title;
}

Curl::Curl()
{
    if (CURLE_OK == curl_global_init(CURL_GLOBAL_DEFAULT))
    {
        _isInitiliazed = true;
    } else {
        LOG_ERROR("Failed to initialize curl.");
    }
}

std::string Curl::strHttpError(const long& errorCode)
{
    std::stringstream message;
    message << errorCode << " - ";

    switch (errorCode) {
        case 400l:
            message << "Bad Request";
            break;
        case 401l:
            message << "Unauthorized";
            break;
        case 403l:
            message << "Forbidden";
            break;
        case 404l:
            message << "Not Found";
            break;
        case 500l:
            message << "Internal Server Error";
            break;
        case 502l:
            message << "Bad Gateway";
            break;
        case 503l:
            message << "Service Unavailable";
            break;
        case 504l:
            message << "Bad Gateway";
            break;
        default:
            message << "The internet is your friend";
    }
    return message.str();
}

std::string Curl::getHttpHeaderField(std::stringstream& headers, const std::string& key)
{
    std::string value;
    std::string line;

    std::string pattern = std::string(key + ": ");

    while (std::getline(headers, line)) {
        size_t o = line.find(pattern);
        if (o != std::string::npos) {
            value = line.substr(o + pattern.size());
            break;
        }
    }
    return value;
}

}
