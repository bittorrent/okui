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

    setMenu(ui::Menu({
        ui::MenuItem("File", ui::Menu({
            ui::MenuItem("Quit", ui::kCommandQuit, KeyCode::kQ, platform->defaultShortcutModifier()),
        })),
        ui::MenuItem("Edit", ui::Menu({
            ui::MenuItem("Copy", ui::kCommandCopy, KeyCode::kC, platform->defaultShortcutModifier()),
            ui::MenuItem("Paste", ui::kCommandPaste, KeyCode::kV, platform->defaultShortcutModifier()),
        })),
    }));
}

Application::~Application() {
    _backgroundRunLoop.cancel();
    _backgroundThread.join();

    _backgroundRunLoop.flush();
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

void Application::async(std::function<void()> task) {
    _platform->async(std::move(task));
}

void Application::backgroundTask(std::function<void()> task) {
    _backgroundRunLoop.async(task);
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

}}
