#pragma once

#include "onair/okui/config.h"

#include "onair/okui/applications/Native.h"
#include "onair/okui/Responder.h"

#include <unordered_map>
#include <thread>
#include <typeindex>
#include <map>

namespace onair {
namespace okui {

class Application : public applications::Native {
public:
    /**
    * @param name human readable application name. e.g. "Example Application"
    * @param organization human readable organization name. e.g. "My Organization"
    * @param resourceManager the resource manager. this is typically provided by subclass implementations
    */
    Application(std::string name, std::string organization, ResourceManager* resourceManager = nullptr);
    virtual ~Application();

    ResourceManager* resourceManager() { return _resourceManager; }
    void setResourceManager(ResourceManager* resourceManager) { _resourceManager = resourceManager; }

    virtual std::string name() const override { return _name; }
    virtual std::string organization() const override { return _organization; }

    std::shared_ptr<std::string> loadResource(const char* name) { return _resourceManager->load(name); }

    // Responder overrides
    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

    /**
    * Sets the CA bundle path to be used for secure requests made by the application.
    */
    void setCABundlePath(std::string caBundlePath) { _caBundlePath = std::move(caBundlePath); }

    /**
    * Returns the current CA bundle path.
    */
    const std::string& caBundlePath() const { return _caBundlePath; }

    /**
    * Asynchronously downloads from the given URL.
    *
    * @param useCache if true, the results of a previously successful download may be provided
    */
    std::future<std::shared_ptr<const std::string>> download(const std::string& url, bool useCache = true);

    /**
    * Used by views to post messages to listeners.
    */
    void post(View* sender, std::type_index index, const void* message, View::Relation relation);

    /**
    * Used by views to listed for posted messages.
    */
    void addListener(View* view, std::type_index index, std::function<void(const void*, View*)>* action, View::Relation relation);
    void removeListeners(View* view);

private:
    struct DownloadInfo {
        std::mutex mutex;
        std::shared_ptr<const std::string> result = nullptr;
        int inProgress                            = 0;
        std::vector<std::promise<std::shared_ptr<const std::string>>> promises;
    };

    std::string _name;
    std::string _organization;
    ResourceManager* _resourceManager;
    std::unique_ptr<ResourceManager> _ownedResourceManager;
    std::string _caBundlePath;
    std::unordered_map<std::string, std::shared_ptr<DownloadInfo>> _downloads;
    std::vector<std::future<void>> _backgroundTasks;

    struct Listener {
        Listener(View* view, std::function<void(const void*, View*)>* action, View::Relation relation)
            : view{view}, action{action}, relation{relation} {}

        View* view;
        std::function<void(const void*, View*)>* action;
        View::Relation relation;
    };

    std::multimap<std::type_index, Listener> _listeners;
};

} // namespace okui
} // namespace onair
