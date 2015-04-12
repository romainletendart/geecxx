/*
 * Copyright (c) 2015, Romain Létendart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "bot.h"

#include <boost/regex/pattern_except.hpp>
#include <boost/regex.hpp>
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>

#include "stringutils.h"

#include "connection.h"
#include "logger.h"
#include "webinforetriever.h"

namespace geecxx
{

Bot::Bot()
{
}

Bot::~Bot()
{
}

bool Bot::init(std::unique_ptr<ConfigurationProvider> configurationProvider)
{
    if (!configurationProvider) {
        LOG_ERROR("Couldn't initialize bot, configuration provider parameter is null");
        return false;
    }
    _configurationProvider.swap(configurationProvider);
    if (!_urlHistory.initFromFile()) {
        LOG_ERROR("Couldn't initialize bot, history file is invalid");
        return false;
    }
    if (_configurationProvider->useSSL()) {
        LOG_ERROR("SSL not supported yet");
        return false;
    } else {
        _connection.reset(new Connection(_configurationProvider->getServer(), std::to_string(_configurationProvider->getPortNumber())));
    }
    if (!_connection) {
        LOG_ERROR("Couldn't initialize bot, connection object is null");
        return false;
    }
    _connection->setExternalReadHandler([this](const std::string& message){
        this->readHandler(message);
    });

    return true;
}

bool Bot::run()
{
    if (!_connection) {
        LOG_ERROR("Couldn't run bot, connection object is null");
        return false;
    }
    if (!_connection->open()) {
        return false;
    }

    // TODO Manage failures of NICK and JOIN commands
    nick(_configurationProvider->getNickname());
    join(_configurationProvider->getChannelName(), _configurationProvider->getChannelKey());

    std::thread cliThread([this]() {
        openCli();
    });

    if (!_connection->listen()) {
        LOG_ERROR("Couldn't run bot, listening on the connection failed");
        // TODO Find a proper way to terminate the thread in that case
        return false;
    }

    cliThread.join();

    return true;
}

void Bot::nick(const std::string& nickname)
{
    const std::string nickCmd("NICK " + nickname);
    const std::string userCmd("USER " + nickname + " * * :" + nickname);

    LOG_INFO(nickCmd);
    LOG_INFO(userCmd);

    std::lock_guard<std::mutex> lock(_connectionMutex);
    _connection->writeMessage(nickCmd);
    _connection->writeMessage(userCmd);
    _nickname = nickname;
}

void Bot::join(const std::string& channel, const std::string& key)
{
    LOG_INFO(std::string("JOIN ") + channel + " " + key);

    std::lock_guard<std::mutex> lock(_connectionMutex);
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

    std::lock_guard<std::mutex> lock(_connectionMutex);
    while (std::getline(stream, messageChunk)) {
        if (messageChunk != "") {
            _connection->writeMessage(std::string("PRIVMSG ") + receiver + " :" + messageChunk);
        }
    }
}

void Bot::pong(const std::string& serverName)
{
    std::lock_guard<std::mutex> lock(_connectionMutex);
    _connection->writeMessage(std::string("PONG ") + serverName);
}

void Bot::quit()
{
    std::lock_guard<std::mutex> lock(_connectionMutex);
    _urlHistory.saveToFile();
    if (_connection && _connection->isAlive()) {
        _connection->writeMessage(std::string("QUIT : Shutting down."));
        _connection->close();
    }
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

        if (recipient == _nickname) {
            return;
        }

        std::vector<std::string> urlCandidates;
        if (parseURL(message, urlCandidates)) {
            for (std::string& url : urlCandidates) {
                processURL(url, sender, recipient);
            }
        }
    } else if (command == "PING") {
        skipToContent(iss);
        std::string host;
        iss >> host;
        pong(host);
    }

}

void Bot::openCli(void)
{
    std::string line, comm;
    bool running = true;
    while (running) {
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
            running = false;
        }
    }
}

bool Bot::parseURL(const std::string& message, std::vector<std::string>& results)
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

    boost::sregex_iterator regexIterator(message.begin(), message.end(), urlRegex);
    const boost::sregex_iterator endIterator = boost::sregex_iterator{};
    bool containsURL = regexIterator != endIterator;
    results.clear();
    while (regexIterator != endIterator) {
        results.push_back((*regexIterator)[0]);
        ++regexIterator;
    }

    return containsURL;
}

void Bot::processURL(const std::string& url, const std::string& sender, const std::string& recipient)
{
    LOG_DEBUG("Found URL: " + url);

    UrlHistoryEntry historyEntry;
    bool alreadyPosted = _urlHistory.find(url, historyEntry);
    if (!alreadyPosted) {
        // First time the URL has been posted, we first need to
        // retrieve the title
        std::string title;
        if (!WebInfoRetriever::getInstance().retrievePageTitle(url, title)) {
            title = "";
        } // else title already set

        // Add the URL to our history
        _urlHistory.insert(url, title, sender);
        if (0 == (_urlHistory.getSize() % _maxUnsavedUrlCount)) {
            _urlHistory.saveToFile();
        }
        _urlHistory.find(url, historyEntry);
    }

    std::stringstream titleOutput;
    std::string urlOutput = stringutils::shorten(stringutils::formatUrl(url), _maxUrlDisplayLength);
    if (historyEntry._title != "") {
        titleOutput << historyEntry._title << " (URL: " << urlOutput << ")";
    }

    if (alreadyPosted) {
        titleOutput << std::endl << sender << ": Already posted by " << historyEntry._messageAuthor;
        titleOutput << " (URL: " << urlOutput << ")";
    }

    if (recipient == _currentChannel) {
        say(titleOutput.str());
    } else {
        msg(sender, titleOutput.str());
    }
}

}

