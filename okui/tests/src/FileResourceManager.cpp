#include "gtest/gtest.h"

#include "onair/okui/FileResourceManager.h"

#include <boost/filesystem.hpp>

using namespace scraps;
using namespace onair::okui;

TEST(FileResourceManager, loading) {
#ifdef SCRAPS_ANDROID
    std::string path{"/sdcard/Download/FileResourceManager.loading"};
#else
    std::string path = (boost::filesystem::temp_directory_path() / "FileResourceManager.loading").string();
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
