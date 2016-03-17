#include "gtest/gtest.h"

#include "onair/okui/config.h"

#include "onair/HTTPConnection.h"
#include "onair/TCPAcceptor.h"

#include "TestApplication.h"

using namespace onair;
using namespace onair::okui;

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

    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(*result, "expected");
}

TEST(Application, downloadCaching) {
    TestApplication application;
    TestServer server;
    server.start(Address(), 12654);

    gTestServerResponse = "response 1";
    auto dl1 = application.download("http://localhost:12654/");
    auto result1 = dl1.get();
    ASSERT_TRUE(result1 != nullptr);

    gTestServerResponse = "response 2";
    auto dl2 = application.download("http://localhost:12654/");
    auto result2 = dl2.get();
    ASSERT_TRUE(result2 != nullptr);

    EXPECT_EQ(*result1, *result2);

    gTestServerResponse = "response 3";
    auto dl3 = application.download("http://localhost:12654/", false);
    auto result3 = dl3.get();
    ASSERT_TRUE(result3 != nullptr);

    EXPECT_NE(*result2, *result3);
}
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
