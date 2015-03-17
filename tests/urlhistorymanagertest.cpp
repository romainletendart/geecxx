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
#include "urlhistorymanagertest.h"

#include "urlhistorymanager.h"

namespace geecxx
{

CPPUNIT_TEST_SUITE_REGISTRATION(UrlHistoryManagerTest);

void UrlHistoryManagerTest::setUp()
{
}

void UrlHistoryManagerTest::tearDown()
{
}

// Actual tests
void UrlHistoryManagerTest::testAll()
{
    UrlHistoryManager history;

    // Test if we can still insert elements after reaching MaxSize
    // (should be possible)
    for (size_t i = 1; i <= 2 * history.getMaxSize(); ++i) {
        std::string url = "URL_" + std::to_string(i);
        std::string title = "Title_" + std::to_string(i);
        std::string messageAuthor = "Author_" + std::to_string(i);
        UrlHistoryEntry entry;
        CPPUNIT_ASSERT_EQUAL(true, history.insert(url, title, messageAuthor));
        // We should not be able to insert more than once
        // the same URL
        CPPUNIT_ASSERT_EQUAL(false, history.insert(url, title, messageAuthor));
        // If we can find the url, the insertion went fine
        CPPUNIT_ASSERT_EQUAL(true, history.find(url, entry));
        CPPUNIT_ASSERT_EQUAL(title, entry._title);
        CPPUNIT_ASSERT_EQUAL(messageAuthor, entry._messageAuthor);
    }
    // Test that we didn't go over MaxSize elements
    CPPUNIT_ASSERT_EQUAL(history.getMaxSize(), history.getSize());

    // Half of the elements we inserted should not exist anymore
    for (size_t i = 1; i <= history.getMaxSize(); ++i) {
        std::string url = "URL_" + std::to_string(i);
        UrlHistoryEntry entry;
        CPPUNIT_ASSERT_EQUAL(false, history.find(url, entry));
    }

    // The other last half should still be in the history
    for (size_t i = history.getMaxSize() + 1;
         i <= 2 * history.getMaxSize();
         ++i) {
        std::string url = "URL_" + std::to_string(i);
        std::string expectedTitle = "Title_" + std::to_string(i);
        UrlHistoryEntry entry;
        CPPUNIT_ASSERT_EQUAL(true, history.find(url, entry));
        CPPUNIT_ASSERT_EQUAL(i - history.getMaxSize(), entry._id);
    }

    UrlHistoryEntry entry;
    CPPUNIT_ASSERT_EQUAL(false, history.find("Unknown URL", entry));

    history.clear();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), history.getSize());
}

}
