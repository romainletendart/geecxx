#ifndef GEECXX_CURL_H
#define GEECXX_CURL_H

#include <string>
#include <sstream>

namespace geecxx
{

class Curl
{
public:
    /**
     * Copy constructor (deleted)
     */
    Curl(const Curl&) = delete;

    /**
     * Move constructor (deleted)
     */
    Curl(Curl &&) = delete;

    /**
     * Copy assignment (deleted)
     */
    Curl& operator=(const Curl&) = delete;

    /**
      * Destructor
      */
    ~Curl();

    /**
     * Get singleton instance
     * @return singleton instance
     */
    static Curl& getInstance();

    /**
     *
     * @param url
     * @return
     */
    std::string retrievePageTitle(const std::string& url);

private:
    /**
     * Default constructor
     */
    Curl();

    std::string strHttpError(const long& errorCode);
    std::string getHttpHeaderField(std::stringstream& headers, const std::string& key);

    bool _isInitiliazed = false;
};

}

#endif //GEECXX_CURL_H
