#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Menu.h"
#include "onair/okui/ResourceManager.h"
#include "onair/okui/Responder.h"
#include "onair/okui/Platform.h"

#include "onair/TaskQueue.h"

#include <unordered_map>
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
    ~Application();

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
    * Downloads from the given URL.
    *
    * @param useCache if true, the results of a previously successful download may be provided
    */
    std::future<std::shared_ptr<const std::string>> download(const std::string& url, bool useCache = true);

    /**
    * Returns task scheduler which runs tasks on the main ui thread
    */
    AbstractTaskScheduler& taskScheduler() { return _platform->taskQueue(); }

    void run();

    void quit();

    // Responder overrides
    virtual Responder* nextResponder() override;
    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

    /**
    * Synchronously downloads from the given URL.
    */
    static std::shared_ptr<const std::string> Download(const std::string& url);

private:
    std::string _name;
    std::string _organization;
    Platform* const _platform;
    ResourceManager* const _resourceManager;
    
    struct DownloadInfo {
        std::shared_ptr<const std::string> result = nullptr;
        std::vector<std::promise<std::shared_ptr<const std::string>>> promises;
    };
    std::unordered_map<std::string, DownloadInfo> _downloads;
    
    std::vector<std::future<void>> _backgroundTasks;
};

}}
