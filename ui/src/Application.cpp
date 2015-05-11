#include "bittorrent/ui/Application.h"

#include "bittorrent/ui/Platform.h"
#include "bittorrent/thread.h"

namespace bittorrent {
namespace ui {

Application::Application(const char* name, const char* organization, Platform* platform, ResourceManager* resourceManager)
    : _name(name), _organization(organization), _platform(platform), _resourceManager(resourceManager)
{
    _backgroundThread = std::thread([this]{
        SetThreadName("Application RunLoop");
        _backgroundRunLoop.run();
    });
}

Application::~Application() {
    _backgroundRunLoop.cancel();
    _backgroundThread.join();

    _backgroundRunLoop.flush();
}

std::string Application::userStoragePath() const {
    return _platform->userStoragePath(_name.c_str(), _organization.c_str());
}

void Application::async(std::function<void()> task) {
    _platform->async(std::move(task));
}

void Application::backgroundTask(std::function<void()> task) {
    _backgroundRunLoop.async(task);
}

void Application::run() {
    _platform->run();
}

}}
