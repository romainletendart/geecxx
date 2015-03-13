#ifndef GEECXX_WEB_INFO_RETRIEVER_H
#define GEECXX_WEB_INFO_RETRIEVER_H

#include <sstream>
#include <string>

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
     * Default constructor
     */
    WebInfoRetriever();

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

    HTMLEntitiesHelper _htmlEntitiesHelper;
    bool _isInitiliazed = false;
};

}

#endif //GEECXX_WEB_INFO_RETRIEVER_H
