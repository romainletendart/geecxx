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
#include "urlhistorymanager.h"

#include <algorithm>

namespace geecxx
{

UrlHistoryManager::UrlHistoryManager(size_t maxSize) : _maxSize(maxSize)
{
}

size_t UrlHistoryManager::getMaxSize()
{
    return _maxSize;
}

size_t UrlHistoryManager::getSize()
{
    return _entries.size();
}

bool UrlHistoryManager::insert(const std::string& url, std::string title, std::string messageAuthor)
{
    std::string formattedUrl = formatUrl(url);
    if (_entries.end() != _entries.find(formattedUrl)) {
        // Entry already exists for given URL, that's an error
        return false;
    }

    if (getSize() == getMaxSize()) {
        // Retrieve oldest entry and remove it from our history
        _entries.erase(_history.front());
        _history.pop();
    }

    UrlHistoryEntry newEntry{getNextId(), std::move(title), std::move(messageAuthor)};
    _history.push(formattedUrl);
    _entries.emplace(std::move(formattedUrl), std::move(newEntry));

    return true;
}

bool UrlHistoryManager::find(const std::string& url, UrlHistoryEntry& entry)
{
    std::string formattedUrl = formatUrl(url);
    auto iterator = _entries.find(formattedUrl);
    if (_entries.end() == iterator) {
        return false;
    }

    entry = iterator->second;
    return true;
}

void UrlHistoryManager::clear()
{
    _entries.clear();
    while (!_history.empty()) {
        _history.pop();
    }
}

size_t UrlHistoryManager::getNextId()
{
    size_t id = _nextId;

    if (_nextId < _maxSize) {
        ++_nextId;
    } else {
        _nextId = 1;
    }

    return id;
}

std::string UrlHistoryManager::formatUrl(const std::string& url)
{
    size_t startIndex = url.find("//");
    if (std::string::npos != startIndex) {
        // Protocol has been found, skip it
        startIndex += 2;
    } else {
        // Protocol not found, stay at the begining
        startIndex = 0;
    }

    // Try to find any fragment at the end of the URL.
    // As we want to modify the string after that point,
    // we need a copy of the current substring.
    size_t endIndex = url.rfind("#");
    std::string formattedUrl;
    if (std::string::npos == endIndex) {
        formattedUrl = url.substr(startIndex);
    } else {
        formattedUrl = url.substr(startIndex, endIndex - startIndex);
    }

    // Set domain name to lower case
    endIndex = formattedUrl.find("/");
    if (std::string::npos != endIndex) {
        std::transform(formattedUrl.begin(), formattedUrl.begin() + endIndex,
                       formattedUrl.begin(), tolower);
    } else {
        std::transform(formattedUrl.begin(), formattedUrl.end(),
                       formattedUrl.begin(), tolower);
    }

    startIndex = formattedUrl.find("www.");
    if (std::string::npos != startIndex) {
        // "www." has been found, skip it
        startIndex += 4;
    } else {
        startIndex = 0;
    }

    return formattedUrl.substr(startIndex);
}

}
