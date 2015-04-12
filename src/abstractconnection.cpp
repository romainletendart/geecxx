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
#include "abstractconnection.h"

#include "logger.h"

namespace geecxx
{

AbstractConnection::AbstractConnection(const std::string& addr, const std::string& port)
    : _addr(addr), _port(port)
{
}

AbstractConnection::~AbstractConnection()
{
}

bool AbstractConnection::open()
{
    if (!connect()) {
        return false;
    }

    // Initialize read handler
    asyncRead();

    return true;
}

bool AbstractConnection::listen()
{
    if (!isAlive()) {
        LOG_ERROR("Cannot listen on closed connection");
        return false;
    }

    // This blocking call waits for completion of all asynchronous calls
    _ioService.run();

    return true;
}

void AbstractConnection::setExternalReadHandler(const ReadHandler& externalReadHandler)
{
    if (!externalReadHandler) {
        LOG_ERROR("Cannot set connection read handler to an empty one");
    } else {
        _externalReadHandler = externalReadHandler;
    }
}

void AbstractConnection::readHandler(const boost::system::error_code& error, std::size_t count)
{
    if (error) {
        close();
    } else {
        std::istream responseStream(&_responseBuffer);
        std::string response;
        std::getline(responseStream, response);
        _externalReadHandler(response);
        asyncRead();
    }
}

}
