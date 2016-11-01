#include "okui/FileResourceManager.h"

#include <fstream>

namespace okui {

std::shared_ptr<std::string> FileResourceManager::load(stdts::string_view name) {
    auto hashable = std::string(name);

    if (auto hit = _cache.get(hashable)) {
        return hit;
    }

    auto path = _directory + "/";
    path.append(name.data(), name.size());
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

} // namespace okui
