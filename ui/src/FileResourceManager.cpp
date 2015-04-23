#include "bittorrent/ui/FileResourceManager.h"

#include <fstream>

namespace bittorrent {
namespace ui {

std::shared_ptr<std::string> FileResourceManager::load(const char* name) {
    auto hashable = std::string(name);

    if (auto hit = _cache.get(hashable)) {
        return hit;
    }

    std::ifstream f(_directory + "/" + name);

    if (!f.is_open()) {
        BT_LOG_ERROR("error opening resource: %d", name);
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
