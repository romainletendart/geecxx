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
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <iostream>

#include "bot.h"
#include "configurationprovider.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    geecxx::Bot bot;

    std::unique_ptr<geecxx::ConfigurationProvider> configurationProvider(new geecxx::ConfigurationProvider{});

    if (!configurationProvider->parseCommandLineArgs(argc, argv)) {
        std::cerr << configurationProvider->help();
        return -1;
    }

    if (configurationProvider->needsHelp()) {
        std::cout << configurationProvider->help();
        return 0;
    }

    if (!bot.init(std::move(configurationProvider))) {
        return -1;
    }

    int exitStatus = 0;
    LOG_INFO("Starting Geecxx...");
    if (!bot.run()) {
        exitStatus = -1;
    }
    LOG_INFO("Geecxx stopped.");

    return exitStatus;
}
