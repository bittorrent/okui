#include "TestApplication.h"

#include <okui/config.h>

#include <scraps/net/HTTPConnection.h>
#include <scraps/net/TCPAcceptor.h>

#include <gtest/gtest.h>

using namespace scraps;
using namespace scraps::net;
using namespace okui;

namespace {
    std::string gTestServerResponse;
}

struct TestHTTPConnection : HTTPConnection {
    using HTTPConnection::HTTPConnection;
    virtual void handleRequest(const Request& request) override {
        sendResponse("HTTP/1.1 200 OK", gTestServerResponse.data(), gTestServerResponse.size());
    }
};

struct TestServer : TCPAcceptor<TestHTTPConnection> {
    std::string response;
};

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(Application, download) {
    TestApplication application;
    TestServer server;
    server.start(Address(), 12654);

    gTestServerResponse = "expected";
    auto result = application.download("http://localhost:12654/").get();

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, "expected");
}

TEST(Application, downloadCaching) {
    TestApplication application;
    TestServer server;
    server.start(Address(), 12654);

    gTestServerResponse = "response 1";
    auto dl1 = application.download("http://localhost:12654/");
    auto result1 = dl1.get();
    ASSERT_TRUE(result1);

    gTestServerResponse = "response 2";
    auto dl2 = application.download("http://localhost:12654/");
    auto result2 = dl2.get();
    ASSERT_TRUE(result2);

    EXPECT_EQ(*result1, *result2);

    gTestServerResponse = "response 3";
    auto dl3 = application.download("http://localhost:12654/", false);
    auto result3 = dl3.get();
    ASSERT_TRUE(result3);

    EXPECT_NE(*result2, *result3);
}

TEST(Application, downloadCachePurging) {
    TestApplication application;
    TestServer server;
    server.start(Address(), 12654);

    gTestServerResponse = "x";
    auto dl1 = application.download("http://localhost:12654/");
    auto result1 = dl1.get();
    ASSERT_TRUE(result1);

    EXPECT_EQ(application.cachedDownloads(), 1);
    size_t individualResponseSize = application.downloadCacheSize();

    auto dl2 = application.download("http://localhost:12654/");
    auto result2 = dl2.get();
    ASSERT_TRUE(result2);

    EXPECT_EQ(application.downloadCacheSize(), individualResponseSize);

    auto dl3 = application.download("http://localhost:12654/3");
    auto result3 = dl3.get();
    ASSERT_TRUE(result3);

    auto dl4 = application.download("http://localhost:12654/4");
    auto result4 = dl4.get();
    ASSERT_TRUE(result4);

    EXPECT_EQ(application.downloadCacheSize(), individualResponseSize * 3);

    // this should do nothing
    application.purgeDownloadCache(10 * individualResponseSize);
    EXPECT_EQ(application.downloadCacheSize(), individualResponseSize * 3);

    // this should purge all but dl4
    application.purgeDownloadCache(individualResponseSize);
    EXPECT_EQ(application.downloadCacheSize(), individualResponseSize);
    gTestServerResponse = "y";
    EXPECT_EQ(*application.download("http://localhost:12654/4").get(), "x");

    // this should purge everything
    application.purgeDownloadCache(0);
    EXPECT_EQ(application.downloadCacheSize(), 0);
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
