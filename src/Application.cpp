#include <okui/Application.h>

#include <okui/Window.h>

#include <scraps/thread.h>
#include <scraps/URL.h>
#include <scraps/net/curl.h>
#include <scraps/net/HTTPRequest.h>

namespace okui {

Application* gDefaultApplication = nullptr;

Application* DefaultApplication() {
    return gDefaultApplication;
}

Application::Application() {
    scraps::net::InitializeCURL();

    gDefaultApplication = this;
}

Application::~Application() {
    if (gDefaultApplication == this) {
        gDefaultApplication = nullptr;
    }

    for (auto& task : _backgroundTasks) {
        task.wait();
    }
}

void Application::bringAllWindowsToFront() {
    for (auto& window : _windows) {
        bringWindowToFront(window);
    }
}

Responder* Application::firstResponder() {
    return activeWindow() ? activeWindow()->firstResponder() : this;
}

void Application::command(Command command, CommandContext context) {
    firstResponder()->chainHandleCommand(command, context);
}

bool Application::canHandleCommand(Command command) {
    return command == kCommandQuit ||
           command == kCommandToggleFullscreenWindow ||
           command == kCommandToggleMinimizeWindow ||
           command == kCommandToggleMaximizeWindow ||
           command == kCommandBringAllWindowsToFront;
}

void Application::handleCommand(Command command, CommandContext context) {
    switch (command) {
        case kCommandQuit:
            quit();
            break;
        case kCommandToggleFullscreenWindow:
            if (activeWindow()) {
                isWindowFullscreen(activeWindow()) ? restoreWindow(activeWindow()) : fullscreenWindow(activeWindow());
            }
            break;
        case kCommandToggleMaximizeWindow:
            if (activeWindow()) {
                isWindowMaximized(activeWindow()) ? restoreWindow(activeWindow()) : maximizeWindow(activeWindow());
            }
            break;
        case kCommandToggleMinimizeWindow:
        if (activeWindow()) {
                isWindowMinimized(activeWindow()) ? restoreWindow(activeWindow()) : minimizeWindow(activeWindow());
            }
            break;
        case kCommandBringAllWindowsToFront:
            bringAllWindowsToFront();
            break;
        default:
            break;
    }
}

std::future<std::shared_ptr<const std::string>> Application::download(const std::string& url, bool useCache) {
    purgeDownloadCache(_maxDownloadCacheSize);

    auto it = _downloads.emplace(url, std::make_shared<DownloadInfo>()).first;
    _downloadUseOrder.remove(it);
    _downloadUseOrder.push_back(it);

    auto download = it->second;
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

        scraps::net::HTTPRequest request;
        request.setCABundlePath(std::move(caBundlePath));
        request.initiate(url);
        request.wait();
        if (request.responseStatus() != 200) {
            SCRAPS_LOGF_WARNING("response code %d from %s", request.responseStatus(), url);
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

void Application::post(View* sender, std::type_index index, const void* message, Relation relation) {
    auto range = _listeners.equal_range(index);
    std::vector<std::function<void(const void*, View*)>*> actions;
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.view->hasRelation(relation, sender) && sender->hasRelation(it->second.relation, it->second.view)) {
            actions.push_back(it->second.action);
        }
    }
    _runActions(actions, sender, index, message);
}

void Application::addListener(View* view, std::type_index index, std::function<void(const void*, View*)>* action, Relation relation) {
    _listeners.emplace(index, Listener(view, action, relation));
}

void Application::removeListeners(View* view) {
    for (auto it = _listeners.begin(); it != _listeners.end();) {
        if (it->second.view == view) {
            it = _listeners.erase(it);
        } else {
            ++it;
        }
    }
}

size_t Application::downloadCacheSize() const {
    size_t ret = 0;
    for (auto& kv : _downloads) {
        ret += kv.second->size();
    }
    return ret;
}

void Application::purgeDownloadCache(size_t maxSize) {
    size_t remainingSize = downloadCacheSize();
    if (remainingSize <= maxSize) { return; }

    for (auto it = _downloadUseOrder.begin(); it != _downloadUseOrder.end();) {
        auto dlIt = *it;
        std::lock_guard<std::mutex> lock(dlIt->second->mutex);
        if (!dlIt->second->inProgress) {
            remainingSize -= dlIt->second->size();
            _downloads.erase(dlIt);
            it = _downloadUseOrder.erase(it);
        } else {
            ++it;
        }
        if (remainingSize <= maxSize) { return; }
    }
}

void Application::_update(Window* window) {
    window->_update();
}

void Application::_render(Window* window) {
    window->_render();
}

void Application::_didResize(Window* window, int width, int height) {
    window->_didResize(width, height);
}

void Application::_assignWindowSize(Window* window) {
    getWindowSize(window, &window->_width, &window->_height);
}

void Application::_post(std::type_index index, const void* message) {
    auto range = _listeners.equal_range(index);
    std::vector<std::function<void(const void*, View*)>*> actions;
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.relation == Relation::kHierarchy || it->second.relation == Relation::kAny || it->second.relation == Relation::kAncestor) {
            actions.push_back(it->second.action);
        }
    }
    _runActions(actions, nullptr, index, message);
}

void Application::_runActions(std::vector<std::function<void(const void*, View*)>*> actions, View* sender, std::type_index index, const void* message) {
    for (auto action : actions) {
        auto range = _listeners.equal_range(index);
        // check that the action was not removed
        if (std::find_if(range.first, range.second, [action](auto& listener) {
                return listener.second.action == action;
            }) != range.second) {
            (*action)(message, sender);
        }
    }
}

void Application::handleURL(std::string url) {
   SCRAPS_LOG_INFO("attempting to handle url: {}", url);
   post(scraps::URL{std::move(url)});
}

} // namespace okui
