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
#pragma once

#include <string>

namespace geecxx
{
namespace stringutils
{

/**
 * Finds pattern in a string (no case-sensitive)
 *
 * @note similar to std::string::find
 * @param[in] haystack string in which the pattern should be searched for
 * @param[in] needle string to search for
 * @return position of the first character of the first match or
 *         std::string::npos if the pattern couldn't be found
 */
size_t findNoCase(const std::string& haystack, const std::string& needle);

/**
 * Make a string displayable on a single line
 *
 * Replace any space character such like '\n', '\r', '\t', '\v', '\f' or ' '
 * with a ' ' and remove extra spaces (more than one space between 2
 * consecutive words).
 * @param s string to be modified
 */
void formatInline(std::string& s);

/**
 * Return formatted and minimized URL
 *
 * The idea behind this function is to minimize URLs by removing
 * protocol information (http(s)://), "www" and fragments (#fragment-id)
 * from them. It also sets the URL to lowercase (only for domain name
 * part).
 * @param[in] url url that will be formatted
 * @return formatted and minimized URL
 */
std::string formatUrl(const std::string& url);

/**
 * Return a shorter representation of the string
 *
 * This function creates a new string out of s which fits the given maxSize.
 * To do so, it replaces the necessary amount of characters in the middle of
 * the string with "[..]". If the string is already short enough, it is
 * returned as is. If it doesn't contain enough characters to cover "[..]",
 * the substring [s[0], s[maxSize - 1]] is returned.
 * @param[in] s string out of which a short representation has to be created
 * @param[in] maxSize maximum size of the shorter representation
 * @return shorter representation of the input string
 */
std::string shorten(const std::string& s, size_t maxSize);

/**
 * Remove any space characters from the beginning of a string
 *
 * Remove any space character that matches against std::isspace.
 * @param[in,out] s string to be modified
 */
void trimLeft(std::string& s);

/**
 * Remove any space characters at the end of a string
 *
 * Remove any space character that matches against std::isspace.
 * @param[in,out] s string to be modified
 */
void trimRight(std::string& s);

/**
 * Remove space characters from both beginning and end of a string
 *
 * @see stringutils::trimLeft and stringutils::trimRight
 * @param[in,out] s string to be modified
 */
void trim(std::string& s);

}
}
