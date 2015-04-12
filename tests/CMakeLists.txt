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
project(GeecxxTest)

set(TARGET "geecxx-test")

set(GEECXX_TEST_DATA_DIR  "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/${TARGET}/")
configure_file(${GeecxxTest_SOURCE_DIR}/testconfig.h.in ${GeecxxTest_BINARY_DIR}/testconfig.h)

include_directories (${Geecxx_SOURCE_DIR}/src ${Geecxx_BINARY_DIR}/src ${GeecxxTest_BINARY_DIR})
include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

set(CONNECTION_TEST_SRCS
    connectiontest.cpp
    ${Geecxx_SOURCE_DIR}/src/connection.cpp
)

set(URL_HISTORY_MANAGER_TEST_SRCS
    urlhistorymanagertest.cpp
    ${Geecxx_SOURCE_DIR}/src/stringutils.cpp
    ${Geecxx_SOURCE_DIR}/src/urlhistorymanager.cpp
)

set(GEECXXTEST_SRCS main.cpp
    ${Geecxx_SOURCE_DIR}/src/logger.cpp
    ${CONNECTION_TEST_SRCS}
    ${URL_HISTORY_MANAGER_TEST_SRCS}
)


add_executable(${TARGET} ${GEECXXTEST_SRCS})
target_link_libraries(${TARGET} ${CPPUNIT_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

install (TARGETS ${TARGET} DESTINATION bin)
install(DIRECTORY DESTINATION ${GEECXX_TEST_DATA_DIR})
