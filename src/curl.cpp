#include <sstream>
#include <string>
#include <curl/curl.h>
#include "curl.h"
#include "logger.h"

namespace
{

static size_t curlWriteCallBack(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::ostringstream*)userp)->write((char*)contents, size * nmemb);
    return size * nmemb;
}

}

namespace geecxx
{

std::string Curl::retrievePageTitle(const std::string& url)
{
    CURL *curl;
    CURLcode res;
    std::ostringstream readBuffer;
    std::string title = "";

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ::curlWriteCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if(res == CURLE_OK) {
            //parsing
            size_t pos = readBuffer.str().find("<title>") + 7;
            size_t pos2 = readBuffer.str().find("</title>");

            if (pos != std::string::npos && pos2 != std::string::npos) {
                title = readBuffer.str().substr(pos, pos2 - pos);
            }
        } else {
            title = std::string(curl_easy_strerror(res));
        }
    } else {
       LOG_ERROR("Failed to allocate curl object.");
    }
    return title;
}

}