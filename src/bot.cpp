#include "bot.h"

#include <boost/regex/pattern_except.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <string>
#include <sstream>

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
    if(_connection == nullptr) {
        return 1;
    }
    _connection->setExternalReadHandler([this](const std::string& m){
        this->_readHandler(m);
    });
    _connection->setExternalWriteHandler([this]() { _writeHandler(); });
    if(!_connection->open()) {
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

void Bot::pong(const std::string& serverName)
{
    _connection->writeMessage(std::string("PONG ") + serverName);
}

void Bot::quit()
{
    _connection->writeMessage(std::string("QUIT : Shutting down."));
}

void Bot::_readHandler(const std::string& message)
{
    LOG_DEBUG("Reading: " + message);

    std::string result;
    if (parseCommand(message)) {
        LOG_DEBUG("Command execution complete");
    } else if (parseURL(message, result)) {
        LOG_DEBUG("Found URL: " + result);
    }
}

void Bot::_writeHandler()
{
    std::string line, comm;
    while (_connection->isAlive()) {
        std::getline(std::cin, line);
        std::istringstream iss(line);

        iss >> comm;
        if (comm == "/n") {
            iss >> comm;
            nick(comm);
        } else if (comm == "/j") {
            iss >> comm;
            join(comm);
        } else if (comm == "/m") {
            iss >> comm;
            msg(comm, iss.str());
        } else if (comm == "/s") {
            say(iss.str());
        } else if (comm == "/q") {
            iss >> comm;
            quit();
        }
    }
}

bool Bot::parseCommand(const std::string& message)
{
    std::istringstream iss(message);
    std::string command;

    iss >> command;

    char c = 0;
    while(!iss.eof() && c != ':') {
        iss >> c;
    }

    bool commandFound = false;
    if(command == "PING") {
        std::string host;
        iss >> host;
        pong(host);
        commandFound = true;
    }

    return commandFound;
}

bool Bot::parseURL(const std::string& message, std::string& result)
{
    boost::regex urlRegex;
    try {
        // Source of the regular expression:
        // http://daringfireball.net/2010/07/improved_regex_for_matching_urls
        urlRegex.assign(R"((?:https?://|www\d{0,3}[.]|[a-z0-9.\-]+[.][a-z]{2,4}/)(?:[^\s()<>]+|\(([^\s()<>]+|(\([^\s()<>]+\)))*\))+(?:\(([^\s()<>]+|(\([^\s()<>]+\)))*\)|[^\s`!()\[\]{};:'".,<>?«»“”‘’]))",
                         boost::regex::ECMAScript);
    } catch(boost::regex_error& error) {
        std::cerr << "Invalid regular expression: " << error.what() << std::endl;
        return false;
    }

    boost::smatch matchedElements;
    bool containsURL = boost::regex_search(message, matchedElements, urlRegex);
    result = matchedElements[0];

    return containsURL;
}

}

