#include "onair/okui/Application.h"

#include "onair/okui/Window.h"

#include "scraps/thread.h"
#include "scraps/net/curl.h"
#include "scraps/net/HTTPRequest.h"

namespace onair {
namespace okui {

Application::Application() {
    scraps::net::InitCURLThreadSafety();
}

Application::~Application() {
    for (auto& task : _backgroundTasks) {
        task.wait();
    }
}

Responder* Application::firstResponder() {
    return activeWindow() ? activeWindow()->firstResponder() : this;
}

void Application::command(Command command, CommandContext context) {
    if (chainCanHandleCommand(command)) {
        firstResponder()->handleCommand(command, context);
    }
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
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.view->hasRelation(relation, sender) && sender->hasRelation(it->second.relation, it->second.view)) {
            (*it->second.action)(message, sender);
        }
    }
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
        ret += sizeof(kv) + (kv.second->result ? kv.second->result->size() : 0);
    }
    return ret;
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

} // namespace okui
} // namespace onair
