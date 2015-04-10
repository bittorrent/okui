#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/RunLoop.h"

#include "bittorrent/ui/Cache.h"
#include "bittorrent/ui/MouseButton.h"
#include "bittorrent/ui/ResourceManager.h"

#include <thread>
#include <unordered_map>

namespace bittorrent {
namespace ui {
    
class Window;

class Application {
public:
    Application(ResourceManager* resourceManager);
    ~Application();
    
    shared_ptr<std::string> loadResource(const char* name) { return _resourceManager->load(name); }

    void backgroundTask(std::function<void()> task);
    
    void run();

private:
    friend class Window;

    ResourceManager* const _resourceManager;
    
    RunLoop _backgroundRunLoop;
    std::thread _backgroundThread;
    
    std::unordered_map<uint32_t, Window*> _windows;

    static MouseButton sMouseButton(uint8_t id);
};

}}