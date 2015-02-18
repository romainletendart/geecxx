#ifndef GEECXX_BOT_H
#define GEECXX_BOT_H

#include "connection.h"
#include <string>

namespace geecxx
{

class Bot
{
public:
    Bot();
    ~Bot();
    int init(const std::string& addr, const std::string& port, const std::string& nickname, const std::string& channel);
    void run();
    void nick(const std::string& nickname);
    void join(const std::string& channel);
    void say(const std::string& message);
    void msg(const std::string& receiver, const std::string& message);
    void pong(const std::string& message);
    void quit();
private:
    bool parseCommand(const std::string& message);
    bool parseURL(const std::string& message, std::string& result);
    void _readHandler(const std::string& message);
    void _writeHandler(void);
    Connection *_connection;
    std::string _currentChannel;
};

}
#endif //GEECXX_BOT_H 
