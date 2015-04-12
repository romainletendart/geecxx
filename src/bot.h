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
#ifndef GEECXX_BOT_H
#define GEECXX_BOT_H

#include <memory>
#include <string>
#include <sstream>
#include <vector>

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
    bool parseURL(const std::string& message, std::vector<std::string>& results);
    void processURL(const std::string& url, const std::string& sender, const std::string& recipient);
    std::istringstream& skipToContent(std::istringstream& iss);
    void readHandler(const std::string& message);
    void openCli(void);

    const size_t _maxUnsavedUrlCount = 10;
    const size_t _maxUrlDisplayLength = 30;

    std::unique_ptr<Connection> _connection;
    std::unique_ptr<ConfigurationProvider> _configurationProvider;
    UrlHistoryManager _urlHistory;
    std::string _currentChannel;
    std::string _nickname;
};

}
#endif //GEECXX_BOT_H
