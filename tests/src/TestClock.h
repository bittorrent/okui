#pragma once

#include <okui/config.h>

#include <chrono>

struct TestClock {
    using duration = std::chrono::seconds;
    using time_point = std::chrono::time_point<TestClock>;

    static void set(time_point t)    { TestClock::t = t; }
    static void set(duration d)      { t = time_point{d}; }
    static time_point now() noexcept { return t; }

    static time_point t;
};
