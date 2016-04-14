#include "gtest/gtest.h"

#include "onair/okui/Cache.h"

using namespace onair::okui;

TEST(Cache, basicOperation) {
    Cache<std::string> cache;

    EXPECT_EQ(cache.get(1), nullptr);

    auto one = cache.add("one", 1);
    EXPECT_NE(one, nullptr);

    EXPECT_EQ(cache.get(2), nullptr);

    EXPECT_EQ(one, cache.get(1));

    EXPECT_EQ(cache.size(), 1);

    one.reset();

    auto two = cache.add("two", 2);

    EXPECT_EQ(*cache.get(2), "two");

    EXPECT_EQ(cache.size(), 1);

    cache.clear();

    EXPECT_EQ(cache.size(), 0);
}
