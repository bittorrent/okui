#pragma once

#include "onair/okui/config.h"

#include "onair/okui/ApplicationBase.h"
#include "onair/okui/Responder.h"

#include "onair/thread.h"

#include <unordered_map>
#include <thread>
#include <typeindex>
#include <map>

namespace onair {
namespace okui {

template <typename Base>
class Application : public Base {
public:
    /**
    * @param name human readable application name. e.g. "Example Application"
    * @param organization human readable organization name. e.g. "My Organization"
    * @param resourceManager the resource manager. this is typically provided by subclass implementations
    */
    Application(std::string name, std::string organization, ResourceManager* resourceManager = nullptr);
    virtual ~Application();

    virtual ResourceManager* resourceManager() override { return _resourceManager; }
    void setResourceManager(ResourceManager* resourceManager) { _resourceManager = resourceManager; }

    virtual std::string name() const override { return _name; }
    virtual std::string organization() const override { return _organization; }

    // Responder overrides
    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

    virtual std::future<std::shared_ptr<const std::string>> download(const std::string& url, bool useCache = true) override;

    /**
    * Sets the CA bundle path to be used for secure requests made by the application.
    */
    void setCABundlePath(std::string caBundlePath) { _caBundlePath = std::move(caBundlePath); }

    /**
    * Returns the current CA bundle path.
    */
    const std::string& caBundlePath() const { return _caBundlePath; }

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
};

template <typename Base>
inline Application<Base>::Application(std::string name, std::string organization, ResourceManager* resourceManager)
    : _name{std::move(name)}
    , _organization{std::move(organization)}
    , _resourceManager{resourceManager}
{
    Base::setMenu(Menu({
        MenuItem("Edit", Menu({
            MenuItem("Copy", kCommandCopy, KeyCode::kC, this->defaultShortcutModifier()),
            MenuItem("Cut", kCommandCut, KeyCode::kX, this->defaultShortcutModifier()),
            MenuItem("Paste", kCommandPaste, KeyCode::kV, this->defaultShortcutModifier()),
            MenuItem("Select All", kCommandSelectAll, KeyCode::kA, this->defaultShortcutModifier()),
        })),
    }));
    
    if (!_resourceManager) {
        _ownedResourceManager = this->defaultResourceManager();
        if (_ownedResourceManager) {
            _resourceManager = _ownedResourceManager.get();
        }
    }
}

template <typename Base>
inline Application<Base>::~Application() {
    for (auto& task : _backgroundTasks) {
        task.wait();
    }
}

template <typename Base>
std::future<std::shared_ptr<const std::string>> Application<Base>::download(const std::string& url, bool useCache) {
    auto it = _downloads.find(url);
    auto download = it == _downloads.end() ? (_downloads[url] = std::make_shared<DownloadInfo>()) : _downloads[url];
    std::lock_guard<std::mutex> lock(download->mutex);

    if (useCache) {
        if (download->result) {
            std::promise<std::shared_ptr<const std::string>> promise;
            promise.set_value(download->result);
            return promise.get_future();
        } else if (download->inProgress) {
            download->promises.emplace_back();
            return download->promises.back().get_future();
        }
    }

    std::promise<std::shared_ptr<const std::string>> promise;
    auto future = promise.get_future();

    ++download->inProgress;

    for (auto it = _backgroundTasks.begin(); it != _backgroundTasks.end();) {
        if (!it->valid() || it->wait_for(0ms) == std::future_status::ready) {
            it = _backgroundTasks.erase(it);
        } else {
            ++it;
        }
    }

    _backgroundTasks.emplace_back(std::async(std::launch::async, [caBundlePath = _caBundlePath, url, download, promise = std::move(promise)] () mutable {
        std::shared_ptr<std::string> result;

        HTTPRequest request;
        request.setCABundlePath(std::move(caBundlePath));
        request.initiate(url);
        request.wait();
        if (request.responseStatus() != 200) {
            ONAIR_LOGF_WARNING("response code %d from %s", request.responseStatus(), url);
        } else {
            result = std::make_shared<std::string>(request.responseBody());
        }

        std::lock_guard<std::mutex> lock(download->mutex);
        download->result = result;

        promise.set_value(result);
        for (auto& promise : download->promises) {
            promise.set_value(result);
        }

        download->promises.clear();
        --download->inProgress;
    }));

    return future;
}

template <typename Base>
inline bool Application<Base>::canHandleCommand(Command command) {
    return command == kCommandQuit || Base::canHandleCommand(command);
}

template <typename Base>
inline void Application<Base>::handleCommand(Command command, CommandContext context) {
    if (command == kCommandQuit) {
        this->quit();
    } else {
        Base::handleCommand(command, context);
    }
}

} // namespace okui
} // namespace onair
