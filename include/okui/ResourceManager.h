#pragma once

#include "okui/config.h"

#include <stdts/string_view.h>
#include <string>

namespace okui {

class ResourceManager {
public:
    virtual ~ResourceManager() {}

    virtual std::shared_ptr<std::string> load(stdts::string_view name) = 0;
};

} // namespace okui
