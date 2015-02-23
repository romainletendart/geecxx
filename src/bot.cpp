#include "bot.h"

#include <boost/regex/pattern_except.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "connection.h"
#include "logger.h"
#include "curl_easy.h"

namespace geecxx
{

Bot::Bot()
{
}

Bot::~Bot()
{
    if (_connection) {
        quit();
        _connection->close();
    }
}

bool Bot::init(const ConfigurationProvider& configuration)
{
    _connection.reset(new Connection(configuration.getServer(), std::to_string(configuration.getPortNumber())));
    if (!_connection) {
        return false;
    }
    _connection->setExternalReadHandler([this](const std::string& m){
        this->_readHandler(m);
    });
    _connection->setExternalWriteHandler([this]() { _writeHandler(); });
    if(!_connection->open()) {
        return false;
    }
    nick(configuration.getNickname());
    join(configuration.getChannelName(), configuration.getChannelKey());

    return true;
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
    _nickname = nickname;
}

void Bot::join(const std::string& channel, const std::string& key)
{
    LOG_INFO(std::string("JOIN ") + channel + " " + key);
    _connection->writeMessage(std::string("JOIN ") + channel + " " + key);
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

std::istringstream& Bot::_skipToContent(std::istringstream& iss)
{
    char c = 0;
    while(!iss.eof() && c != ':') {
        iss >> c;
    }
    return iss;
}

void Bot::_readHandler(const std::string& message)
{
    LOG_DEBUG("Reading: " + message);

    std::istringstream iss(message);
    std::string command;
    std::string sender = "";
    iss >> command;

    if (command.size() > 1 && command[0] == ':') {
    //this is probably a privmsg
        size_t pos = command.find('!');
        if (pos != std::string::npos) {
            sender = command.substr(1, pos - 1);
            iss >> command;
        }
    }
    
    if (command == "PRIVMSG") {
        std::string recipient;
        iss >> recipient;
        LOG_DEBUG("PRIVMSG FROM " + sender + " TO " + recipient);

        if(recipient == _nickname) {
            return;
        }
        
        std::string result;
        if (_parseURL(message, result)) {
            LOG_DEBUG("Found URL: " + result);
            const std::string title = _getTitleFromUrl(result);
            if (sender == _currentChannel) {
                say(title);
            } else {
                msg(sender, title);
            }
        }
    } else if (command == "PING") {
        _skipToContent(iss);
        std::string host;
        iss >> host;
        pong(host);
    }

}

std::string Bot::_getTitleFromUrl(const std::string& url)
{
    std::ostringstream oss;
    curl_writer writer(oss);
    curl::curl_easy easy(writer);
    easy.add(curl_pair<CURLoption,string>(CURLOPT_URL, url) );
    easy.add(curl_pair<CURLoption,long>(CURLOPT_FOLLOWLOCATION, 1L));
    try {
        easy.perform();
    } catch (curl::curl_easy_exception e) {
        std::vector<pair<std::string, std::string>> w = e.what();
        return "Exception: " + w.at(0).first;
    }
    
    size_t pos = oss.str().find("<title>") + 7;
    size_t pos2 = oss.str().find("</title>");
    return oss.str().substr(pos, pos2 - pos);
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

bool Bot::_parseURL(const std::string& message, std::string& result)
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

