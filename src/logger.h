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
#ifndef GEECXX_LOGGER_H_
#define GEECXX_LOGGER_H_

#include <mutex>
#include <string>

#include "loggerconfig.h"

namespace geecxx
{

enum class LogLevel : std::uint8_t
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:
    Logger(const Logger&) = delete;
    Logger(Logger &&) = delete;
    Logger& operator=(const Logger&) = delete;

    ~Logger();
    static Logger& getInstance();

    void log(const LogLevel& logLevel, const std::string& message);
private:
    Logger();
    std::string logLevelToStr(const LogLevel& logLevel);

    std::mutex _mutex;
    const LogLevel _logLevelThreshold = LOG_LEVEL_THRESHOLD;
};

}

#define LOG_DEBUG(message)   geecxx::Logger::getInstance().log(geecxx::LogLevel::DEBUG, message)
#define LOG_INFO(message)    geecxx::Logger::getInstance().log(geecxx::LogLevel::INFO, message)
#define LOG_WARNING(message) geecxx::Logger::getInstance().log(geecxx::LogLeve::WARNING, message)
#define LOG_ERROR(message)   geecxx::Logger::getInstance().log(geecxx::LogLevel::ERROR, message)

#endif // GEECXX_LOGGER_H_
