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
#include <okui/FileResourceManager.h>

#include <gtest/gtest.h>

using namespace okui;

TEST(FileResourceManager, loading) {
#ifdef SCRAPS_ANDROID
    std::string path{"/sdcard/Download/FileResourceManager_loading"};
#else
    std::string path = "./FileResourceManager_loading";
#endif

    FILE* f = fopen(path.c_str(), "w");
    ASSERT_NE(f, nullptr);
    fprintf(f, "file contents");
    fclose(f);

    auto directory = path.substr(0, path.find_last_of('/'));
    auto name = path.substr(path.find_last_of('/') + 1);

    FileResourceManager frm(directory.c_str());

    auto a = frm.load(name.c_str());
    EXPECT_EQ(*a, "file contents");

    auto b = frm.load(name.c_str());
    EXPECT_EQ(a, b);

    remove(path.c_str());
}
