#include "onair/okui/Application.h"

#include "onair/okui/Platform.h"

#include "onair/HTTPRequest.h"
#include "onair/HTTPSRequest.h"
#include "onair/URL.h"
#include "onair/thread.h"

namespace onair {
namespace okui {

Application::Application(const char* name, const char* organization, Platform* platform, ResourceManager* resourceManager)
    : _name{name}
    , _organization{organization}
    , _platform{platform}
    , _resourceManager{resourceManager}
{
    setMenu(Menu({
        MenuItem("Edit", Menu({
            MenuItem("Copy", kCommandCopy, KeyCode::kC, platform->defaultShortcutModifier()),
            MenuItem("Paste", kCommandPaste, KeyCode::kV, platform->defaultShortcutModifier()),
        })),
    }));
}

Application::~Application() {
    for (auto& task : _backgroundTasks) {
        task.wait();
    }
}

Responder* Application::firstResponder() {
    if (auto window = _platform->activeWindow()) {
        return window->firstResponder();
    }
    return this;
}

void Application::setMenu(const Menu& menu) {
    _platform->setApplicationMenu(this, menu);
}

std::string Application::userStoragePath() const {
    return _platform->userStoragePath(_name.c_str(), _organization.c_str());
}

std::future<std::shared_ptr<const std::string>> Application::download(const std::string& url, bool useCache) {
    auto& download = _downloads[url];
    if (useCache) {
        if (download.result) {
            std::promise<std::shared_ptr<const std::string>> promise;
            promise.set_value(download.result);
            return promise.get_future();
        } else if (!download.promises.empty()) {
            download.promises.emplace_back();
            return download.promises.back().get_future();
        }
    }
    
    std::promise<std::shared_ptr<const std::string>> promise;
    auto future = promise.get_future();

    _backgroundTasks.emplace_back(std::async([=, promise = std::move(promise)] () mutable {
        auto result = Download(url);
        taskScheduler()->async([=, promise = std::move(promise)] () mutable {
            auto& download = _downloads[url];
            download.result = result;

            promise.set_value(result);
            for (auto& promise : download.promises) {
                promise.set_value(result);
            }

            download.promises.clear();
        });
    }));
    
    return future;
}

void Application::run() {
    _platform->run();
}

void Application::quit() {
    _platform->quit();
}

Responder* Application::nextResponder() {
    return _platform;
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
    URL parsed{url};
    
    if (parsed.protocol() == "http") {
        HTTPRequest request(parsed.host().c_str(), parsed.port(), "GET", parsed.resource().c_str());
        request.wait();
        if (request.responseStatus() == 200) {
            return std::make_shared<std::string>(request.responseBody());
        }
    } else if (parsed.protocol() == "https") {
        HTTPSRequest request(parsed.host().c_str(), parsed.port(), "GET", parsed.resource().c_str());
        request.wait();
        if (request.responseStatus() == 200) {
            return std::make_shared<std::string>(request.responseBody());
        }
    }
    
    return nullptr;
}

}}
