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
namespace geecxx
{

UrlHistoryManager::UrlHistoryManager(size_t maxSize) : _maxSize(maxSize)
{
}

size_t UrlHistoryManager::getMaxSize()
{
    return _maxSize;
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

size_t UrlHistoryManager::getSize()
{
    return _entries.size();
}

bool UrlHistoryManager::insert(std::string url, std::string title, std::string messageAuthor)
{
    if (_entries.end() != _entries.find(url)) {
        // Entry already exists for given URL, that's an error
        return false;
    }

    if (getSize() == getMaxSize()) {
        // Retrieve oldest entry and remove it from our history
        _entries.erase(_history.front());
        _history.pop();
    }

    UrlHistoryEntry newEntry{getNextId(), std::move(title), std::move(messageAuthor)};
    _history.push(url);
    _entries.emplace(std::move(url), std::move(newEntry));

    return true;
}

bool UrlHistoryManager::find(const std::string& url, UrlHistoryEntry& entry)
{
    auto iterator = _entries.find(url);
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

}
