#ifndef GEECXX_WEB_INFO_RETRIEVER_H
#define GEECXX_WEB_INFO_RETRIEVER_H

#include <string>
#include <sstream>

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

    std::string retrievePageTitle(const std::string& url);

private:
    /**
     * Default constructor
     */
    WebInfoRetriever();

    std::string strHttpError(const long& errorCode);
    std::string getHttpHeaderField(std::stringstream& headers, const std::string& key);

    bool _isInitiliazed = false;
};

}

#endif //GEECXX_WEB_INFO_RETRIEVER_H
