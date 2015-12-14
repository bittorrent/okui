#include "TestApplication.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

#if !defined(OKUI_TEST_RESOURCES_PATH)
#error You must define OKUI_TEST_RESOURCES_PATH in your build.
#endif

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

using testing::internal::FilePath;

std::string TestApplication::Essentials::ResourceManagerPath() {
#if (ONAIR_IOS || ONAIR_TVOS) && !ONAIR_SIMULATOR
    FilePath path{FilePath::ConcatPaths(FilePath(boost::filesystem::temp_directory_path().string()),
                                        FilePath("okui-tests-" ONAIR_APPLE_SDK "-bundle.xctest"))};

#elif ONAIR_ANDROID
    FilePath path{"/data/local/tmp/resources"};
#else
    FilePath path{OKUI_TEST_RESOURCES_PATH};
#endif

    EXPECT_TRUE(path.FileOrDirectoryExists());
    return path.string();
}

#endif
