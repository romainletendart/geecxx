/*
 * Copyright (c) 2015, Romain Létendart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "webinforetriever.h"

#include <curl/curl.h>
#include <sstream>
#include <string>

#include "logger.h"
#include "stringutils.h"

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

WebInfoRetriever::~WebInfoRetriever()
{
    curl_global_cleanup();
}

WebInfoRetriever& WebInfoRetriever::getInstance()
{
    // Static variable initialization ensures threadsafety in C++11 standard
    static WebInfoRetriever webInfoRetriever;

    return webInfoRetriever;
}

bool WebInfoRetriever::retrievePageTitle(const std::string& url, std::string &pageTitle)
{
    if (!_isInitiliazed) {
        return false;
    }

    long statusCode;
    std::string response;
    // Retrieve headers information first to check Content-Type
    if (!sendHttpRequest(url, RequestType::HEADER, response, statusCode)) {
        return false;
    }

    std::string contentType = getHttpHeaderField(response, "Content-Type");
    if (std::string::npos == contentType.find("text/html")) {
        // Not a HTML document, that's an error
        return false;
    }

    if (statusCode != 200l) {
        // HTML document with wrong HTTP code, we inform the user
        pageTitle = strHttpError(statusCode);
        return true;
    }

    // Retrieve body now that we are safe
    if (!sendHttpRequest(url, RequestType::BODY, response, statusCode)) {
        return false;
    }

    pageTitle = extractTitleFromContent(response);

    return true;
}

WebInfoRetriever::WebInfoRetriever(std::string caFilePath)
    : _caFilePath(caFilePath)
{
    if (CURLE_OK == curl_global_init(CURL_GLOBAL_DEFAULT))
    {
        _isInitiliazed = true;
    } else {
        LOG_ERROR("Failed to initialize curl.");
    }
}

std::string WebInfoRetriever::extractTitleFromContent(const std::string &pageContent)
{
    size_t titleBegin = stringutils::findNoCase(pageContent, "<title>") + 7;
    size_t titleEnd = stringutils::findNoCase(pageContent, "</title>");
    if (std::string::npos == titleBegin || std::string::npos == titleEnd) {
        return "";
    }

    std::string title = pageContent.substr(titleBegin, titleEnd - titleBegin);

    title = _htmlEntitiesHelper.decode(title);
    // Inline formatting should happen after HTML entities decoding that may
    // bring extra white spaces
    stringutils::formatInline(title);

    return title;
}

std::string WebInfoRetriever::getHttpHeaderField(const std::string &headers, const std::string& key)
{
    std::string value;
    std::string line;

    std::string pattern = std::string(key + ": ");

    std::stringstream headersStream(headers);
    while (std::getline(headersStream, line)) {
        size_t o = stringutils::findNoCase(line, pattern);
        if (o != std::string::npos) {
            value = line.substr(o + pattern.size());
            break;
        }
    }

    return value;
}

bool WebInfoRetriever::sendHttpRequest(const std::string &url, RequestType type, std::string &response, long& statusCode)
{
    CURL *curl;
    CURLcode res;
    std::stringstream readBuffer;

    curl = curl_easy_init();
    if (nullptr == curl) {
        LOG_ERROR("Failed to allocate curl object.");
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ::curlWriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, _caFilePath.c_str());
    if (RequestType::HEADER == type) {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    }

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        LOG_ERROR("CURL request failed for URL: " + url);
        LOG_ERROR("Reason: " + std::string(curl_easy_strerror(res)));
        return false;
    }

    response = readBuffer.str();

    return true;
}

std::string WebInfoRetriever::strHttpError(const long& errorCode)
{
    std::stringstream message;
    message << "HTTP " << errorCode << " - ";

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
    case 429l:
        message << "Too many requests";
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
        message << "No description available";
    }

    return message.str();
}

}
