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
#ifndef GEECXX_ABSTRACT_CONNECTION_H
#define GEECXX_ABSTRACT_CONNECTION_H

#include <boost/asio.hpp>
#include <functional>
#include <string>

namespace geecxx {

typedef std::function<void (const std::string&)> ReadHandler;

class AbstractConnection
{
public:
    AbstractConnection(const std::string& addr, const std::string& port);
    virtual ~AbstractConnection();

    bool listen();
    bool open();
    void readHandler(const boost::system::error_code& error, std::size_t);
    void setExternalReadHandler(const ReadHandler& externalReadHandler);

    virtual void close() = 0;
    virtual bool isAlive() const = 0;
    virtual bool writeMessage(const std::string& message) = 0;

protected:
    virtual void asyncRead() = 0;
    virtual bool connect() = 0;

    std::string _addr;
    std::string _port;

    boost::asio::io_service _ioService;

    /**
     * External handler to be called when data are available for reading.
     * Default handler does nothing.
     */
    ReadHandler _externalReadHandler = [](const std::string&) {};

    boost::asio::streambuf _responseBuffer;
};

}

#endif // GEECXX_ABSTRACT_CONNECTION_H
