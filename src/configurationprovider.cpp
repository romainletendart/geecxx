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
#include "configurationprovider.h"

#include <istream>
#include <sstream>
#include <string>
#include <vector>

namespace geecxx
{

bool ConfigurationProvider::parseCommandLineArgs(int argc, char *argv[])
{
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i) {
        arguments.emplace_back(argv[i]);
    }

    size_t curIndex = 0;
    if (curIndex == arguments.size()) {
        return false;
    }

    _server = arguments[curIndex];

    if (++curIndex == arguments.size()) {
        return false;
    }

    uint16_t tempPortNumber;
    std::istringstream(arguments[curIndex]) >> tempPortNumber;

    // Even if 0 was the port number entered by the user, it is still an
    // invalid TCP port number.
    if (0 == tempPortNumber) {
        return false;
    }

    _portNumber = tempPortNumber;

    if (++curIndex == arguments.size()) {
        return false;
    }

    _channelName = arguments[curIndex];

    if (++curIndex == arguments.size()) {
        return true;
    }

    _channelKey = arguments[curIndex];

    if (++curIndex == arguments.size()) {
        return true;
    }

    _nickname = arguments[curIndex];

    return (++curIndex == arguments.size());
}

std::string ConfigurationProvider::help()
{
    std::stringstream help;

    // TODO Use target name instead of hard coded program name
    help << "Usage: geecxx SERVER PORT CHANNEL [KEY] [NICKNAME]" << std::endl;
    help << "Run geecxx bot on SERVER:PORT under the given NICKNAME on a specific CHANNEL." << std::endl;
    help << std::endl;
    help << "Mandatory arguments are:" << std::endl;
    help << "SERVER\t\t\t Name or IP address of the IRC server" << std::endl;
    help << "PORT\t\t\t TCP port number on which the IRC server is running" << std::endl;
    help << "CHANNEL\t\t\t Channel that the bot has to join" << std::endl;
    help << std::endl;
    help << "Optional arguments are:" << std::endl;
    help << "KEY\t\t\t Channel key that the bot shoud provide to enter the specific CHANNEL (if needed)" << std::endl;
    help << "NICKNAME\t\t\t Nickname that the bot has to use" << std::endl;

    return help.str();
}

std::string ConfigurationProvider::getServer() const
{
    return _server;
}

std::uint16_t ConfigurationProvider::getPortNumber() const
{
    return _portNumber;
}

std::string ConfigurationProvider::getNickname() const
{
    return _nickname;
}

std::string ConfigurationProvider::getChannelName() const
{
    return _channelName;
}

std::string ConfigurationProvider::getChannelKey() const
{
    return _channelKey;
}

}
