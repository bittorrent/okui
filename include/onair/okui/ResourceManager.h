#pragma once

#include "onair/okui/config.h"

#include <string>

namespace onair {
namespace okui {

class ResourceManager {
public:
    virtual ~ResourceManager() {}

    virtual std::shared_ptr<std::string> load(const char* name) = 0;
};

} } //namespace onair::okui
