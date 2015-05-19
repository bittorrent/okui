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
    /**
    * @param name human readable application name. e.g. "Example Application"
    * @param organization human readable organization name. e.g. "My Organization"
    */
    Application(const char* name, const char* organization, Platform* platform, ResourceManager* resourceManager);
    ~Application();

    Platform* platform() const { return _platform; }

    std::shared_ptr<std::string> loadResource(const char* name) { return _resourceManager->load(name); }
        
    /**
    * Returns a path suitable for storing preferences or other persistent user data.
    *
    * For example, on OS X this might return "/Users/user1/Library/Application Support/My Organization/Example Application"
    */
    std::string userStoragePath() const;

    /**
    * Queues a task to be invoked on the main thread asynchronously.
    *
    * Thread-safe.
    */
    void async(std::function<void()> task);

    void backgroundTask(std::function<void()> task);

    void run();
    
    void quit();

private:
    std::string _name;
    std::string _organization;
    Platform* const _platform;
    ResourceManager* const _resourceManager;

    RunLoop _backgroundRunLoop;
    std::thread _backgroundThread;
};

}}
