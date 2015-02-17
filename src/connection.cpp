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
#include <thread>

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

    _socket.async_read_some(boost::asio::buffer(_buffer),
        boost::bind(&Connection::readHandler, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

    // This blocking call waits for completion of all asynchronous calls
    //_ioService.run();

    return true;
}

void Connection::close()
{
    _socket.close();
    _ioService.stop();
}

bool Connection::isAlive() const
{
    return _socket.is_open();
}

void Connection::setExternalReadHandler(const ReadHandler& externalReadHandler)
{
    _externalReadHandler = externalReadHandler;
}

void Connection::setExternalWriteHandler(const WriteHandler& externalWriteHandler)
{
    _externalWriteHandler = externalWriteHandler;
}

void Connection::writeMessage(const std::string& message)
{
    boost::asio::write(_socket, boost::asio::buffer(message + "\r\n"));
}

void Connection::readHandler(const boost::system::error_code& error, std::size_t count)
{
    if (error) {
        close();
    } else {
        _externalReadHandler(std::string(_buffer.data(), count));

        _socket.async_read_some(boost::asio::buffer(_buffer),
            boost::bind(&Connection::readHandler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
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
        return false;
    }

    LOG_INFO("Connected.");
    return true;
}

void Connection::run()
{
    std::thread writeHandlerThread(_externalWriteHandler);
    _socket.async_read_some(boost::asio::buffer(_buffer),
        boost::bind(&Connection::readHandler, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
        )
    );
    _ioService.run();
    writeHandlerThread.join();
}

}
