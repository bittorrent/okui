#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Menu.h"
#include "onair/okui/ResourceManager.h"
#include "onair/okui/Responder.h"
#include "onair/okui/Platform.h"

#include "onair/TaskQueue.h"

#include <thread>

namespace onair {
namespace okui {

class Application : public Responder {
public:
    /**
    * @param name human readable application name. e.g. "Example Application"
    * @param organization human readable organization name. e.g. "My Organization"
    */
    Application(const char* name, const char* organization, Platform* platform, ResourceManager* resourceManager);

    Platform* platform() const { return _platform; }

    std::shared_ptr<std::string> loadResource(const char* name) { return _resourceManager->load(name); }

    /**
    * The application-wide first responder is the active window's first responder or the application if
    * there are no open windows.
    */
    Responder* firstResponder();

    /**
    * Sets an application-wide menu. On some platforms, this is used as an alternative to window-specific menus.
    */
    void setMenu(const Menu& menu);

    /**
    * Returns a path suitable for storing preferences or other persistent user data.
    *
    * For example, on OS X this might return "/Users/user1/Library/Application Support/My Organization/Example Application"
    */
    std::string userStoragePath() const;

    /**
    * Returns task scheduler which runs tasks on the main ui thread
    */
    TaskQueue& taskQueue() { return _platform->taskQueue(); }

    void run();

    void quit();

    // Responder overrides
    virtual Responder* nextResponder() override;
    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

private:
    std::string _name;
    std::string _organization;
    Platform* const _platform;
    ResourceManager* const _resourceManager;
};

}}
