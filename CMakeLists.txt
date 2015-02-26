# Copyright (c) 2015, Romain Létendart
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
cmake_minimum_required(VERSION 2.8.11)

project(Geecxx)

set(TARGET "geecxx")

# Options
option(WITH_TESTS "Generate tests binaries and data" OFF)

# Find required packages
find_package(Boost 1.53 COMPONENTS locale program_options regex system REQUIRED)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig)
if(WITH_TESTS)
    pkg_check_modules(CPPUNIT REQUIRED cppunit>=1.13)
endif()

# Initialize CXXFLAGS
set(CMAKE_CXX_FLAGS       "-Wall -Werror -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# Configuration
if(NOT WITH_LOG_LEVEL)
    set(WITH_LOG_LEVEL 1 CACHE STRING "Log level threshold (0=DEBUG;1=INFO;2=WARNING;3=ERROR)")
endif()

configure_file(${Geecxx_SOURCE_DIR}/src/loggerconfig.h.in ${Geecxx_BINARY_DIR}/src/loggerconfig.h)

add_subdirectory(src)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()
