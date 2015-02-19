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
#include "logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>

namespace geecxx
{

Logger::~Logger()
{
}

Logger& Logger::getInstance()
{
    // Static variable initialization ensures threadsafety in C++11 standard
    static Logger logger;
    return logger;
}

void Logger::log(const LogLevel& logLevel, const std::string& message)
{
    if (logLevel < _logLevelThreshold) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    std::time_t now = std::chrono::system_clock::to_time_t(
                                std::chrono::system_clock::now());
    char formattedTime[20];

    if (0 == strftime(formattedTime, 20, "%F %T", localtime(&now))) {
        // Be sure to display an empty string instead of undefined content
        formattedTime[0] = '\0';
    }

    std::istringstream stream(message);
    std::string messageChunk;
    while (std::getline(stream, messageChunk) && !messageChunk.empty()) {
        std::cout << "[" << formattedTime << "][" << logLevelToStr(logLevel) << "]: "
                  << messageChunk << std::endl;
    }
}

Logger::Logger()
{
}

std::string Logger::logLevelToStr(const LogLevel& logLevel)
{
    switch (logLevel) {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }

    return "";
}

}
