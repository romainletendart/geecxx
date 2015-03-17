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
#include <map>
#include <queue>
#include <string>

namespace geecxx
{

struct UrlHistoryEntry
{
    size_t _id;
    std::string _title;
    std::string _messageAuthor;
};

class UrlHistoryManager
{
public:
    /**
     * Constructor
     *
     * @param maxSize maximum number of elements in the history
     */
    UrlHistoryManager(size_t maxSize = 512);

    /**
     * Get maximum size of the history
     * @return maximum size of the history
     */
    size_t getMaxSize();

    /**
     * Get current size of the history
     * @return current size of the history
     */
    size_t getSize();

    /**
     * Insert a new entry in the history
     *
     * @param[in] url url to be used as the key of the new element
     * @param[in] title title associated to the given url
     * @param[in] messageAuthor author of the message that contains the given url
     * @return true upon successful insertion, false otherwise
     */
    bool insert(std::string url, std::string title, std::string messageAuthor);

    /**
     * Find an entry in the history
     *
     * @param[in] url url to be used as the key to retrieve the element
     * @param[out] entry
     * @return true if the element has been found, false otherwise
     */
    bool find(const std::string& url, UrlHistoryEntry& entry);

    /**
     * Remove every single entry of the history
     */
    void clear();

private:
    /**
     * Return id not yet used by any element
     *
     * Also increases internal id for successive calls.
     * @return id not yet used by any element
     */
    size_t getNextId();

    /**
     * Maximum size of the history
     */
    const size_t _maxSize;

    /**
     * Id to be used for new element to be inserted
     */
    size_t _nextId = 1;

    /**
     * Proper storage of the history's entries
     * key = URL
     */
    std::map<std::string, UrlHistoryEntry> _entries;

    /**
     * Keeps track of the order in which entries were inserted
     */
    std::queue<std::string> _history;
};

}
