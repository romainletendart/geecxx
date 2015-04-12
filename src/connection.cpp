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
#include "connection.h"

#include <boost/bind.hpp>

#include "logger.h"

namespace geecxx
{

Connection::Connection(const std::string& addr, const std::string& port)
    : _addr(addr), _port(port), _socket(_ioService)
{
}

Connection::~Connection()
{
    close();
}

bool Connection::open()
{
    if (!connect()) {
        return false;
    }

    // Initialize read handler
    asyncRead();

    return true;
}

void Connection::close()
{
    _ioService.stop();
    if (_socket.is_open()) {
        _socket.shutdown(boost::asio::socket_base::shutdown_both);
        _socket.close();
    }
}

bool Connection::listen()
{
    if (!isAlive()) {
        LOG_ERROR("Cannot listen on closed connection");
        return false;
    }

    // This blocking call waits for completion of all asynchronous calls
    _ioService.run();

    return true;
}

bool Connection::isAlive() const
{
    return _socket.is_open();
}

void Connection::setExternalReadHandler(const ReadHandler& externalReadHandler)
{
    if (!externalReadHandler) {
        LOG_ERROR("Cannot set connection read handler to an empty one");
    } else {
        _externalReadHandler = externalReadHandler;
    }
}

bool Connection::writeMessage(const std::string& message)
{
    if (!isAlive()) {
        LOG_ERROR("Cannot write on closed connection");
        return false;
    }

    boost::asio::write(_socket, boost::asio::buffer(message + "\r\n"));
    return true;
}

void Connection::readHandler(const boost::system::error_code& error, std::size_t count)
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

bool Connection::connect()
{
    boost::asio::ip::tcp::resolver resolver(_ioService);
    boost::asio::ip::tcp::resolver::query query(_addr, _port);

    boost::asio::ip::tcp::resolver::iterator iter;

    // Default initialization is enough here
    boost::asio::ip::tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;
    LOG_INFO("Trying to connect to " + _addr + ":" + _port + "...");

    // As the resolver may return more than one result, we try to connect to
    // every end point until one of those connections succeeds
    for (iter = resolver.resolve(query); iter != end && error; ++iter) {
        _socket.connect(*iter, error);
    }

    if (error) {
        LOG_ERROR("Couldn't connect to " + _addr + ":" + _port + ".");
        LOG_ERROR("Reason: " + error.message());
        close();
        return false;
    }

    LOG_INFO("Connected.");
    return true;
}

void Connection::asyncRead()
{
    boost::asio::async_read_until(_socket, _responseBuffer,
        "\r\n", boost::bind(&Connection::readHandler, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

}
