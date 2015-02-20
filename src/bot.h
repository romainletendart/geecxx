#ifndef GEECXX_BOT_H
#define GEECXX_BOT_H

#include <memory>
#include <string>
#include <sstream>

#include "configurationprovider.h"
#include "connection.h"

namespace geecxx
{

class Bot
{
public:
    Bot();
    ~Bot();
    bool init(const ConfigurationProvider& configuration);
    void run();
    void nick(const std::string& nickname);
    void join(const std::string& channel, const std::string &key = "");
    void say(const std::string& message);
    void msg(const std::string& receiver, const std::string& message);
    void pong(const std::string& message);
    void quit();
private:
    bool _parseURL(const std::string& message, std::string& result);
    std::istringstream& _skipToContent(std::istringstream& iss);
    void _readHandler(const std::string& message);
    void _writeHandler(void);
    std::unique_ptr<Connection> _connection;
    std::string _currentChannel;
};

}
#endif //GEECXX_BOT_H
