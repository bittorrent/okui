#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Cache.h"
#include "bittorrent/ui/MouseButton.h"
#include "bittorrent/ui/ResourceManager.h"

#include <unordered_map>

namespace bittorrent {
namespace ui {
    
class Window;

class Application {
public:
    Application(ResourceManager* resourceManager) : _resourceManager(resourceManager) {}
    virtual ~Application() {}
    
    shared_ptr<std::string> loadResource(const char* name) { return _resourceManager->load(name); }
    
    void run();

private:
    friend class Window;

    ResourceManager* const _resourceManager;
    
    std::unordered_map<uint32_t, Window*> _windows;

    static MouseButton sMouseButton(uint8_t id);
};

}}