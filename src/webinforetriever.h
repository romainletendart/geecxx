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
#pragma once

#include <sstream>
#include <string>

#include "globalconfig.h"

#include "htmlentitieshelper.h"

namespace geecxx
{

class WebInfoRetriever
{
public:
    /**
     * Copy constructor (deleted)
     */
    WebInfoRetriever(const WebInfoRetriever&) = delete;

    /**
     * Move constructor (deleted)
     */
    WebInfoRetriever(WebInfoRetriever &&) = delete;

    /**
     * Copy assignment (deleted)
     */
    WebInfoRetriever& operator=(const WebInfoRetriever&) = delete;

    /**
      * Destructor
      */
    ~WebInfoRetriever();

    /**
     * Get singleton instance
     * @return singleton instance
     */
    static WebInfoRetriever& getInstance();

    /**
     * Retrieve web page title from the given URL
     *
     * This function first checks that the given URL leads to a valid HTML
     * document alongside with the HTTP status code 200. If so, it then looks
     * for its title (content of <title> tag) and fills pageTitle with such
     * content. In case no <title> tag can be found or its content is empty,
     * pageTitle will be set to "".
     *
     * HTTP status codes different from 200 lead to pageTitle being set to an
     * explanatory message describing the error (only in a case of a HTML
     * document).
     *
     * Following cases are considered as errors and will imply the function to
     * return false:
     * - URLs pointing to anything but a HTML document
     * - Internal error while retrieving information from the server
     *
     * In those error cases, pageTitle content is undefined.
     * @param[in] url URL of the web page
     * @param[out] pageTitle web page title (might be empty)
     * @return true upon successful title retrieval
     */
    bool retrievePageTitle(const std::string& url, std::string& pageTitle);

private:
    /**
     * Constructor
     *
     * @param[in] caFilePath Path to the Certification Authority (CA) file
     */
    WebInfoRetriever(std::string caFilePath = GEECXX_CONF_DIR "ca-bundle.crt");

    enum class RequestType
    {
        HEADER,
        BODY
    };

    /**
     * Extract "<title>" tag content from HTML content
     *
     * The main purpose of this function is to extract the title of a HTML
     * document. If any title has been found it is then formatted. Extra white
     * spaces and new lines are removed and HTML entities are replaced with
     * their equivalent unicode characters.
     * @param[in] pageContent HTML content
     * @return formatted title (if any), otherwise an empty string
     */
    std::string extractTitleFromContent(const std::string &pageContent);

    /**
     * Retrieve a specific field from HTTP headers
     * @param[in] headers HTTP headers
     * @param[in] key name of the field which needs to be retrieved
     * @return field value
     */
    std::string getHttpHeaderField(const std::string &headers, const std::string& key);

    /**
     * Send a HTTP request to retrieve given URL content
     *
     * @param[in] url URL whose content needs to be retrieved
     * @param[in] type type of response expected (header or body)
     * @param[out] response actual content
     * @param[out] statusCode HTTP status code
     * @return true upon successful request execution, false otherwise
     */
    bool sendHttpRequest(const std::string& url, RequestType type, std::string& response, long& statusCode);

    /**
     * Return a description of a HTTP status code
     * @param[in] errorCode HTTP status code
     * @return HTTP status code description
     */
    std::string strHttpError(const long& errorCode);

    /**
     * Path to the Certification Authority (CA) file that should be used
     * to validate peer certificates
     */
    const std::string _caFilePath;

    HTMLEntitiesHelper _htmlEntitiesHelper;
    bool _isInitiliazed = false;
};

}
