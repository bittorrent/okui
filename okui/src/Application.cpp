#include "onair/okui/Application.h"

#include "onair/okui/Platform.h"
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
