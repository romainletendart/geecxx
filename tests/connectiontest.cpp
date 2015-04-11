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
#include "connectiontest.h"

#include "connection.h"

namespace geecxx
{

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionTest);

void ConnectionTest::setUp()
{
}

void ConnectionTest::tearDown()
{
}

// Actual tests
void ConnectionTest::testObjectConstructionDestruction()
{
    Connection *connection = new Connection("localhost", "6667");

    CPPUNIT_ASSERT(nullptr != connection);

    delete connection;
}

void ConnectionTest::testWriteBeforeOpen()
{
    Connection connection("localhost", "6667");

    // Writing a message on a closed connection should fail
    CPPUNIT_ASSERT_EQUAL(false, connection.writeMessage("BOOM"));
}

void ConnectionTest::testWrongPort()
{
    Connection connectionA("localhost", "0");

    // A connection shouldn't be "alive" before we explicitly open it
    CPPUNIT_ASSERT_EQUAL(false, connectionA.isAlive());
    CPPUNIT_ASSERT_EQUAL(false, connectionA.open());
    CPPUNIT_ASSERT_EQUAL(false, connectionA.isAlive());
}

}
