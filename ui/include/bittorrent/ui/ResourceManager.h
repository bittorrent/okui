#pragma once

#include "bittorrent/ui/config.h"

#include <string>

namespace bittorrent {
namespace ui {

class ResourceManager {
public:
    virtual ~ResourceManager() {}

    virtual std::shared_ptr<std::string> load(const char* name) = 0;
};

}}
