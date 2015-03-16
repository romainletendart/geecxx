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
#include <string>

namespace geecxx
{
namespace stringutils
{

/**
 * Finds pattern in a string (no case-sensitive)
 *
 * @note similar to std::string::find
 * @param haystack string in which the pattern should be searched for
 * @param needle string to search for
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
 * @param s
 */
void trim(std::string& s);

}
}
