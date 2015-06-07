#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Cache.h"
#include "onair/okui/ResourceManager.h"

namespace onair {
namespace okui {

class FileResourceManager : public ResourceManager {
public:
    FileResourceManager(const char* directory) : _directory(directory) {}

    virtual std::shared_ptr<std::string> load(const char* name) override;

private:
    const std::string _directory;
    Cache<std::string> _cache;
};

}}
