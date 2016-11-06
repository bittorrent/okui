#include "TestApplication.h"

#include <gtest/gtest.h>

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

#if !defined(SCRAPS_ANDROID) && !defined(OKUI_TEST_RESOURCES_PATH)
#error You must define OKUI_TEST_RESOURCES_PATH in your build.
#endif

#ifndef SCRAPS_ANDROID

using testing::internal::FilePath;

std::string TestApplication::ResourceManagerPath() {
#if (SCRAPS_IOS || SCRAPS_TVOS) && !SCRAPS_SIMULATOR
    FilePath path{[[[NSBundle mainBundle] resourcePath] UTF8String]};
#else
    FilePath path{OKUI_TEST_RESOURCES_PATH};
#endif

    EXPECT_TRUE(path.FileOrDirectoryExists());
    return path.string();
}

#endif // SCRAPS_ANDROID

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
