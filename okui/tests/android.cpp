#include "onair/okui/config.h"

#if ONAIR_ANDROID

#include <apk-wrapper/gtest-android-helper.h>

APK_WRAPPER_GTEST_MAIN("okui-tests", tv_watchonair_okui_tests, [](auto){})

#endif
