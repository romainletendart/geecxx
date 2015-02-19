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
#ifndef GEECXX_CONFIGURATION_PROVIDER_H_
#define GEECXX_CONFIGURATION_PROVIDER_H_

#include <cstdint>
#include <string>

#include "defines.h"

namespace geecxx
{

/**
 * The ConfigurationProvider class provides a way to easily retrieve bot
 * configuration from command line arguments.
 */
class ConfigurationProvider
{
public:
    /**
     * Default constructor.
     */
    ConfigurationProvider() = default;

    /**
     * Parse command line arguments forwarded from main() to retrieve and store
     * bot configuration.
     * @param[in] argc arguments count forwarded from main()
     * @param[in] argv arguments values forwarded from main()
     * @return true if provided arguments have successfully been translated
     *         to bot configuration
     */
    bool parseCommandLineArgs(int argc, char *argv[]);

    /**
     * Returns help string explaining how to configure the bot.
     * @return help string
     */
    std::string help();

    std::string getServer() const;

    std::uint16_t getPortNumber() const;

    std::string getNickname() const;

    std::string getChannelName() const;

    std::string getChannelKey() const;

private:
    std::string _server = GEECXX_DEFAULT_SERVER;
    std::uint16_t _portNumber = GEECXX_DEFAULT_PORT;
    std::string _nickname = GEECXX_DEFAULT_NICK;
    std::string _channelName = GEECXX_DEFAULT_CHANNEL;
    std::string _channelKey; // Channel key is empty by default
};

}

#endif // GEECXX_CONFIGURATION_PROVIDER_H_
