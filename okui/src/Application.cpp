#include "onair/okui/Application.h"
#include "onair/HTTPRequest.h"
#include "onair/thread.h"

namespace onair {
namespace okui {

Application::Application(const char* name, const char* organization, ResourceManager* resourceManager)
    : _name{name}
    , _organization{organization}
    , _resourceManager{resourceManager}
{}

Application::~Application() {
    for (auto& task : _backgroundTasks) {
        task.wait();
    }
}

Responder* Application::firstResponder() {
    if (auto window = activeWindow()) {
        return window->firstResponder();
    }
    return this;
}

std::future<std::shared_ptr<const std::string>> Application::download(const std::string& url, bool useCache) {
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
        if (!it->valid() || it->wait_for(0_ms) == std::future_status::ready) {
            it = _backgroundTasks.erase(it);
        } else {
            ++it;
        }
    }

    _backgroundTasks.emplace_back(std::async(std::launch::async, [url, download, promise = std::move(promise)] () mutable {
        auto result = Download(url);

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

bool Application::canHandleCommand(Command command) {
    return command == kCommandQuit;
}

void Application::handleCommand(Command command, CommandContext context) {
    if (command == kCommandQuit) {
        quit();
    } else {
        Responder::handleCommand(command, context);
    }
}

std::shared_ptr<const std::string> Application::Download(const std::string& url) {
    HTTPRequest request(url);
    request.wait();
    if (request.responseStatus() != 200) {
        ONAIR_LOG_WARNING("response code %d from %s", request.responseStatus(), url);
        return nullptr;
    }
    return std::make_shared<std::string>(request.responseBody());
}

void Application::_init() {
    setMenu(Menu({
        MenuItem("Edit", Menu({
            MenuItem("Copy", kCommandCopy, KeyCode::kC, defaultShortcutModifier()),
            MenuItem("Cut", kCommandCut, KeyCode::kX, defaultShortcutModifier()),
            MenuItem("Paste", kCommandPaste, KeyCode::kV, defaultShortcutModifier()),
            MenuItem("Select All", kCommandSelectAll, KeyCode::kA, defaultShortcutModifier()),
        })),
    }));

    if (_resourceManager == nullptr) {
        _defaultResourceManager = std::make_unique<okui::FileResourceManager>(resourcePath().c_str());
        _resourceManager = _defaultResourceManager.get();
    }
}

}}
