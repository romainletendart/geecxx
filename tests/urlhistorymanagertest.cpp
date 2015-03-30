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

#include <cstdio>
#include <fstream>

#include "logger.h"
#include "urlhistorymanager.h"

namespace geecxx
{

CPPUNIT_TEST_SUITE_REGISTRATION(UrlHistoryManagerTest);

void UrlHistoryManagerTest::setUp()
{
}

void UrlHistoryManagerTest::tearDown()
{
    std::ifstream historyFile(_historyFilePath);
    if (historyFile.good()) {
        // File exists, we need to delete it
        historyFile.close();
        if (0 != remove(_historyFilePath.c_str())) {
            LOG_ERROR("Unable to remove file: " + _historyFilePath);
        }
    } // else, nothing to clean up
}

// Actual tests
void UrlHistoryManagerTest::testAll()
{
    UrlHistoryManager history;
    const std::string urlPrefix = "http://www.website.com/page";

    // Test if we can still insert elements after reaching MaxSize
    // (should be possible)
    for (size_t i = 1; i <= 2 * history.getMaxSize(); ++i) {
        std::string url = urlPrefix + std::to_string(i);
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
        std::string url = urlPrefix + std::to_string(i);
        UrlHistoryEntry entry;
        CPPUNIT_ASSERT_EQUAL(false, history.find(url, entry));
    }

    // The other last half should still be in the history
    for (size_t i = history.getMaxSize() + 1;
         i <= 2 * history.getMaxSize();
         ++i) {
        std::string url = urlPrefix + std::to_string(i);
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

void UrlHistoryManagerTest::testSimilarUrls()
{
    UrlHistoryManager history;

    // After execution of next line, every insertion of a similar URL should fail
    CPPUNIT_ASSERT_EQUAL(true, history.insert("http://www.website.com/", "", ""));
    // Using https instead of http
    CPPUNIT_ASSERT_EQUAL(false, history.insert("https://www.website.com/", "", ""));
    // Omitting the protocol
    CPPUNIT_ASSERT_EQUAL(false, history.insert("www.website.com/", "", ""));
    // Specifying the protocol, omitting "www"
    CPPUNIT_ASSERT_EQUAL(false, history.insert("http://website.com/", "", ""));
    CPPUNIT_ASSERT_EQUAL(false, history.insert("https://website.com/", "", ""));
    // Omitting both protocol and "www"
    CPPUNIT_ASSERT_EQUAL(false, history.insert("website.com/", "", ""));
    // Adding fragment
    CPPUNIT_ASSERT_EQUAL(false, history.insert("http://www.website.com/#fragment-id", "", ""));
    // Uppercase protocol and domain name
    CPPUNIT_ASSERT_EQUAL(false, history.insert("HTTP://WWW.WEBSITE.COM/", "", ""));
    // Resource name should not be case sensitive
    CPPUNIT_ASSERT_EQUAL(true, history.insert("HTTP://WWW.WEBSITE.COM/RESOURCE1", "", ""));
    CPPUNIT_ASSERT_EQUAL(true, history.insert("HTTP://WWW.WEBSITE.COM/resource1", "", ""));
}

void UrlHistoryManagerTest::testInitFromSaveToFile()
{
    // History size (== 8) is arbitrary here
    UrlHistoryManager history(8, _historyFilePath);

    const std::string urlA = "http://www.websiteA.com/";
    const std::string urlB = "http://www.websiteB.com/";
    const UrlHistoryEntry expectedEntryA = { 1, "Title A", "Author A"};
    const UrlHistoryEntry expectedEntryB = { 2, "Title B", "Author B"};

    CPPUNIT_ASSERT_EQUAL(true, history.insert(urlA, expectedEntryA._title,
                         expectedEntryA._messageAuthor));
    CPPUNIT_ASSERT_EQUAL(true, history.insert(urlB, expectedEntryB._title,
                         expectedEntryB._messageAuthor));
    CPPUNIT_ASSERT_EQUAL(true, history.saveToFile());

    history.clear();
    CPPUNIT_ASSERT_EQUAL(size_t(0), history.getSize());

    CPPUNIT_ASSERT_EQUAL(true, history.initFromFile());
    CPPUNIT_ASSERT_EQUAL(size_t(2), history.getSize());

    UrlHistoryEntry readEntryA;
    UrlHistoryEntry readEntryB;
    CPPUNIT_ASSERT_EQUAL(true, history.find(urlA, readEntryA));
    CPPUNIT_ASSERT_EQUAL(true, history.find(urlB, readEntryB));

    CPPUNIT_ASSERT_EQUAL(expectedEntryA._id, readEntryA._id);
    CPPUNIT_ASSERT_EQUAL(expectedEntryA._title, readEntryA._title);
    CPPUNIT_ASSERT_EQUAL(expectedEntryA._messageAuthor, readEntryA._messageAuthor);

    CPPUNIT_ASSERT_EQUAL(expectedEntryB._id, readEntryB._id);
    CPPUNIT_ASSERT_EQUAL(expectedEntryB._title, readEntryB._title);
    CPPUNIT_ASSERT_EQUAL(expectedEntryB._messageAuthor, readEntryB._messageAuthor);
}

void UrlHistoryManagerTest::testEmptyHistoryFile()
{
    // History size (== 8) is arbitrary here
    UrlHistoryManager history(8, _historyFilePath);

    // Saving empty history to file (should create an empty file)
    CPPUNIT_ASSERT_EQUAL(true, history.saveToFile());

    // We should be able to read from an empty file
    CPPUNIT_ASSERT_EQUAL(true, history.initFromFile());
}

}
