#include "gtest/gtest.h"

#include "bittorrent/ui/FileResourceManager.h"

using namespace bittorrent;
using namespace bittorrent::ui;

TEST(FileResourceManager, loading) {
    std::string path = tmpnam(nullptr);

    FILE* f = fopen(path.c_str(), "w");
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
