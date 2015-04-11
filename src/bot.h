#ifndef GEECXX_BOT_H
#define GEECXX_BOT_H

#include <memory>
#include <string>
#include <sstream>

#include "configurationprovider.h"
#include "connection.h"
#include "urlhistorymanager.h"

namespace geecxx
{

class Bot
{
public:
    Bot();
    ~Bot();
    bool init(std::unique_ptr<ConfigurationProvider> configurationProvider);
    bool run();
    void nick(const std::string& nickname);
    void join(const std::string& channel, const std::string &key = "");
    void say(const std::string& message);
    void msg(const std::string& receiver, const std::string& message);
    void pong(const std::string& message);
    void quit();

private:
    bool parseURL(const std::string& message, std::string& result);
    std::istringstream& skipToContent(std::istringstream& iss);
    void readHandler(const std::string& message);
    void openCli(void);

    const size_t _maxUnsavedUrlCount = 10;

    std::unique_ptr<Connection> _connection;
    std::unique_ptr<ConfigurationProvider> _configurationProvider;
    UrlHistoryManager _urlHistory;
    std::string _currentChannel;
    std::string _nickname;
};

}
#endif //GEECXX_BOT_H
