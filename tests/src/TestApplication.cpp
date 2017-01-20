/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
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
