#include "gtest/gtest.h"

#include "okui/FileResourceManager.h"

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
