#include "bot.h"

#include <boost/regex/pattern_except.hpp>
#include <boost/regex.hpp>
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>

#include "logger.h"
#include "webinforetriever.h"

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
    if (!_urlHistory.initFromFile()) {
        return false;
    }
    _connection.reset(new Connection(configuration.getServer(), std::to_string(configuration.getPortNumber())));
    if (!_connection) {
        return false;
    }
    _connection->setExternalReadHandler([this](const std::string& m){
        this->readHandler(m);
    });
    _connection->setExternalWriteHandler([this]() { writeHandler(); });
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
    std::istringstream stream(message);
    std::string messageChunk;
    while (std::getline(stream, messageChunk)) {
        if (messageChunk != "") {
            _connection->writeMessage(std::string("PRIVMSG ") + receiver + " :" + messageChunk);
        }
    }
}

void Bot::pong(const std::string& serverName)
{
    _connection->writeMessage(std::string("PONG ") + serverName);
}

void Bot::quit()
{
    _urlHistory.saveToFile();
    _connection->writeMessage(std::string("QUIT : Shutting down."));
}

std::istringstream& Bot::skipToContent(std::istringstream& iss)
{
    char c = 0;
    while(!iss.eof() && c != ':') {
        iss >> c;
    }
    return iss;
}

void Bot::readHandler(const std::string& message)
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

        std::string urlCandidate;
        if (parseURL(message, urlCandidate)) {
            LOG_DEBUG("Found URL: " + urlCandidate);

            UrlHistoryEntry historyEntry;
            bool alreadyPosted = _urlHistory.find(urlCandidate, historyEntry);
            if (!alreadyPosted) {
                // First time the URL has been posted, we first need to
                // retrieve the title
                std::string title;
                if (!WebInfoRetriever::getInstance().retrievePageTitle(urlCandidate, title)) {
                    title = "";
                } // else title already set

                // Add the URL to our history
                _urlHistory.insert(urlCandidate, title, sender);
                _urlHistory.find(urlCandidate, historyEntry);
            }

            std::stringstream titleOutput;
            if (historyEntry._title != "") {
                titleOutput << historyEntry._title << " (URL#" << historyEntry._id << ")";
            }

            if (alreadyPosted) {
                titleOutput << std::endl << sender << ": Already posted by " << historyEntry._messageAuthor;
                titleOutput << " (URL#" << historyEntry._id << ")";
            }

            if (recipient == _currentChannel) {
                say(titleOutput.str());
            } else {
                msg(sender, titleOutput.str());
            }
        }
    } else if (command == "PING") {
        skipToContent(iss);
        std::string host;
        iss >> host;
        pong(host);
    }

}

void Bot::writeHandler()
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
            iss.seekg(1, std::ios_base::cur); //skipping space
            std::string message;
            std::getline(iss, message);
            msg(comm, message);
        } else if (comm == "/s") {
            iss.seekg(1, std::ios_base::cur); //skipping space
            std::string message;
            std::getline(iss, message);
            say(message);
        } else if (comm == "/q") {
            iss >> comm;
            quit();
        }
    }
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
        LOG_ERROR("Invalid regular expression: " + std::string(error.what()));
        return false;
    }

    boost::smatch matchedElements;
    bool containsURL = boost::regex_search(message, matchedElements, urlRegex);
    result = matchedElements[0];

    return containsURL;
}

}

