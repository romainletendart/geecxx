#include <string>
#include <sstream>
#include <iostream>
#include "bot.h"
#include "connection.h"
#include "logger.h"

namespace geecxx
{

Bot::Bot()
{
}

Bot::~Bot()
{
    quit();
    _connection->close();
    delete _connection;
}

int Bot::init(const std::string& addr, const std::string& port, const std::string& nickname, const std::string& channel)
{
    _connection = new Connection(addr, port);
    if(_connection == nullptr)
    {
        return 1;
    }
    _connection->setExternalReadHandler([this](const std::string& m){
        this->_readHandler(m);
    });
    _connection->setExternalWriteHandler([this]() { _writeHandler(); });
    if(!_connection->open())
    {
        return 2;
    }
    nick(nickname);
    join(channel);
    return 0;
}

void Bot::run()
{
    _connection->run();
}

void Bot::nick(const std::string& nickname)
{
    LOG_INFO("nick: " + nickname);
    _connection->writeMessage(std::string("NICK ") + nickname);
    _connection->writeMessage(std::string("USER ") + nickname + " * * :" + nickname);
}

void Bot::join(const std::string& channel)
{
    _connection->writeMessage(std::string("JOIN ") + channel);
    _currentChannel = channel;
}

void Bot::say(const std::string& message)
{
   msg(_currentChannel, message); 
}

void Bot::msg(const std::string& receiver, const std::string& message)
{
    _connection->writeMessage(std::string("PRIVMSG ") + receiver + " :" + message);
}

void Bot::quit()
{
    _connection->writeMessage(std::string("QUIT : Shutting down."));
}

void Bot::_readHandler(const std::string& message)
{
    LOG_INFO("Reading: " + message);
}

void Bot::_writeHandler()
{
    std::string line, comm;
    while (_connection->isAlive())
    {
        std::getline(std::cin, line);
        std::istringstream iss(line);

        iss >> comm;
        if (comm == "/n")
        {
            iss >> comm;
            nick(comm);
        }
        else if (comm == "/j")
        {
            iss >> comm;
            join(comm);
        }
        else if (comm == "/m")
        {
            iss >> comm;
            msg(comm, iss.str());
        }
        else if (comm == "/s")
        {
            say(iss.str());
        }
        else if (comm == "/q")
        {
            iss >> comm;
            quit();
        }
    }
}

}
        
