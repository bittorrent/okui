#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/RunLoop.h"

#include "bittorrent/ui/ResourceManager.h"

#include <thread>

namespace bittorrent {
namespace ui {

class Platform;

class Application {
public:
    Application(Platform* platform, ResourceManager* resourceManager);
    ~Application();

    Platform* platform() const { return _platform; }

    std::shared_ptr<std::string> loadResource(const char* name) { return _resourceManager->load(name); }

    /**
    * Thread-safe.
    */
    void async(std::function<void()> task);

    void backgroundTask(std::function<void()> task);

    void run();

private:
    Platform* const _platform;
    ResourceManager* const _resourceManager;

    RunLoop _backgroundRunLoop;
    std::thread _backgroundThread;
};

}}
