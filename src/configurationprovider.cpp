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

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace geecxx
{

ConfigurationProvider::ConfigurationProvider() :
    _help(false)
{
    po::options_description mandatory("Mandatory arguments");
    mandatory.add_options()
        ("server", po::value<std::string>(&_server)->required(), "the irc server to connect to")
        ("port", po::value<std::uint16_t>(&_portNumber)->required(), "the port to connect to")
        ("channel", po::value<std::string>(&_channelName)->required(), "the channel to connect to")
    ;
    po::options_description optional("Optional arguments");
    optional.add_options()
        ("key", po::value<std::string>(&_channelKey)->default_value(std::string()), "the protection key for the channel")
        ("nick", po::value<std::string>(&_nickname)->default_value(std::string("geecxx")), "the bot's nickname")
        ("use-ssl", po::value<bool>(&_useSSL)->default_value(false), "use SSL to communicate with the IRC server")
    ;
    po::options_description generic("Generic options");
    generic.add_options()
        ("help,h", "produce help message")
    ;
    _cliOptions.add(mandatory).add(optional).add(generic);
    _helpOptions.add(optional).add(generic);

}

bool ConfigurationProvider::parseCommandLineArgs(int argc, char *argv[])
{
    try {
        po::positional_options_description p;
        p.add("server", 1);
        p.add("port", 1);
        p.add("channel", 1);

        po::variables_map vm;      
        po::store(po::command_line_parser(argc, argv).options(_cliOptions).positional(p).run(), vm);
 
        if (vm.count("help")) {
            _help = true;
            return true;
        }

        po::notify(vm); 

    } catch (po::required_option& e) {
        return false;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

std::string ConfigurationProvider::help()
{
    std::ostringstream oss;
    oss << "Usage: geecxx [options] <server> <port> <channel>" << std::endl;
    oss << _helpOptions;
    return oss.str();
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

bool ConfigurationProvider::useSSL() const
{
    return _useSSL;
}

bool ConfigurationProvider::needsHelp() const
{
    return _help;
}

}
