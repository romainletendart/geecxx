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
#include "stringutils.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <iterator>
#include <sstream>

namespace geecxx
{
namespace stringutils
{

size_t findNoCase(const std::string& haystack, const std::string& needle)
{
    size_t needleIndex = std::string::npos;
    auto it = std::search(
      haystack.begin(), haystack.end(),
      needle.begin(),   needle.end(),
      [](char c1, char c2) { return std::toupper(c1) == std::toupper(c2); }
    );

    if (it != haystack.end()) {
        needleIndex = std::distance(haystack.begin(), it);
    }

    return needleIndex;
}

void formatInline(std::string& s)
{
    char titleCopy[s.size() + 1];

    // strtok needs to be able to modify the string, hence the copy
    memcpy(titleCopy, s.c_str(), s.size() + 1);

    char *word = nullptr;
    char *strtokContext = nullptr;
    const char delim[] = "\n\r\t\v\f ";
    std::stringstream outputStream;

    // We only need to pass titleCopy on the first call to strtok
    word = strtok_r(titleCopy, delim, &strtokContext);
    while (nullptr != word) {
        outputStream << word;
        word = strtok_r(nullptr, delim, &strtokContext);
        if (nullptr != word) {
            // Separate two consecutive words with a space
            outputStream << " ";
        }
    }

    s = outputStream.str();
}

void trimLeft(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

void trimRight(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

void trim(std::string& s)
{
    trimLeft(s);
    trimRight(s);
}

}
}
