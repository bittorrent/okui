#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Cache.h"
#include "bittorrent/ui/ResourceManager.h"

namespace bittorrent {
namespace ui {

class FileResourceManager : public ResourceManager {
public:
    FileResourceManager(const char* directory) : _directory(directory) {}

    virtual std::shared_ptr<std::string> load(const char* name) override;

private:
    const std::string _directory;
    Cache<std::string> _cache;
};

}}
