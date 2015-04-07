#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Cache.h"

#include <string>

namespace bittorrent {
namespace ui {

class ResourceManager {
public:
    ResourceManager(const char* directory) : _directory(directory) {}
    
    shared_ptr<std::string> load(const char* name);

private:
    const std::string _directory;
    Cache<std::string> _cache;
};

}}