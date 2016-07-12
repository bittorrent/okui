#include "onair/okui/FileResourceManager.h"

#include <fstream>

namespace onair {
namespace okui {

std::shared_ptr<std::string> FileResourceManager::load(const char* name) {
    auto hashable = std::string(name);

    if (auto hit = _cache.get(hashable)) {
        return hit;
    }

    const auto path = _directory + "/" + name;
    std::ifstream f(path);

    if (!f.is_open()) {
        SCRAPS_LOGF_ERROR("error opening resource: %s", path.c_str());
        return nullptr;
    }

    f.seekg(0, std::ios::end);
    auto size = f.tellg();
    std::string buffer(size, ' ');
    f.seekg(0);
    f.read(&buffer[0], size);

    return _cache.add(std::move(buffer), hashable);
}

}}
