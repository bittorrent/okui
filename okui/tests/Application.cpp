#include "gtest/gtest.h"

#include "onair/okui/config.h"

#include "TestApplication.h"

using namespace onair;
using namespace onair::okui;

TEST(Application, Download) {
    auto result = Application::Download("https://www.random.org/integers/?num=1&min=12340&max=12341&col=1&base=10&format=plain");
    
    ASSERT_TRUE(result != nullptr);
    EXPECT_TRUE(*result == "12340\n" || *result == "12341\n");
}

TEST(Application, downloadCaching) {
    TestApplication application;

    auto dl1 = application.download("https://www.random.org/integers/?num=1&min=-1000000000&max=1000000000&col=1&base=10&format=plain");
    auto dl2 = application.download("https://www.random.org/integers/?num=1&min=-1000000000&max=1000000000&col=1&base=10&format=plain");
    
    auto result1 = dl1.get();
    ASSERT_TRUE(result1 != nullptr);

    auto result2 = dl2.get();
    ASSERT_TRUE(result2 != nullptr);

    EXPECT_EQ(*result1, *result2);

    auto dl3 = application.download("https://www.random.org/integers/?num=1&min=-1000000000&max=1000000000&col=1&base=10&format=plain", false);
    auto result3 = dl3.get();
    ASSERT_TRUE(result3 != nullptr);

    // there's a 1 in 2000000001 chance to get a false negative here :-/
    EXPECT_NE(*result2, *result3);
}
