#pragma once

#include "onair/okui/config.h"

#include "onair/okui/ResourceManager.h"

#include "scraps/Cache.h"

namespace onair {
namespace okui {

class FileResourceManager : public ResourceManager {
public:
    explicit FileResourceManager(const char* directory) : _directory(directory) {}

    virtual std::shared_ptr<std::string> load(const char* name) override;

private:
    const std::string _directory;
    scraps::Cache<std::string> _cache;
};

} } //namespace onair::okui
