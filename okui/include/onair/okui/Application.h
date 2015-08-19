#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Menu.h"
#include "onair/okui/ResourceManager.h"
#include "onair/okui/Responder.h"
#include "onair/okui/Menu.h"
#include "onair/okui/Window.h"
#include "onair/okui/CursorTypes.h"
#include "onair/okui/FileResourceManager.h"

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
    * @param resourceManager if left as nullptr, a FileResourceManager is used which uses resourcePath()
    */
    Application(const char* name, const char* organization, ResourceManager* resourceManager = nullptr);
    virtual ~Application();

    void setResourceManager(ResourceManager* resourceManager) { _resourceManager = resourceManager; }

    const std::string& name() const { return _name; }
    const std::string& organization() const { return _organization; }

    std::shared_ptr<std::string> loadResource(const char* name) { return _resourceManager->load(name); }

    /**
    * The application-wide first responder is the active window's first responder or the application if
    * there are no open windows.
    */
    Responder* firstResponder();

    /**
    * Executes the given command. Equivalent to `firstResponder()->handleCommand(command, context)`.
    */
    void command(Command command, CommandContext context) { firstResponder()->handleCommand(command, context); }

    /**
    * Downloads from the given URL.
    *
    * @param useCache if true, the results of a previously successful download may be provided
    */
    std::future<std::shared_ptr<const std::string>> download(const std::string& url, bool useCache = true);

    // Responder overrides
    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

    /**
    * Synchronously downloads from the given URL.
    */
    static std::shared_ptr<const std::string> Download(const std::string& url);

    /**
    * iOS: applicationWillTerminate(), Android: applicationWillTerminate()
    */
    virtual void terminating() {}

    /**
    * iOS: applicationDidReceiveMemoryWarning(), Android: onLowMemory()
    */
    virtual void lowMemory() {}

    /**
    * iOS: applicationWillResignActive(), Android: onPause()
    */
    virtual void enteringBackground() {}

    /**
    * iOS: applicationDidEnterBackground(), Android: onPause()
    */
    virtual void enteredBackground() {}

    /**
    * iOS: applicationWillEnterForeground(), Android: onResume()
    */
    virtual void enteringForeground() {}

    /**
    * iOS: applicationDidBecomeActive(), Android: onResume()
    */
    virtual void enteredForeground() {}

    /**
    * Queues up a tasks to be executed asynchronously (but on the main thread).
    *
    * This method is thread-safe.
    */
    TaskQueue* taskScheduler() { return &_taskQueue; }

    /**
    * Override this to set up the main run loop. The run loop must regularly call work().
    */
    virtual void run() = 0;

    /**
    * A call to this method should cause the main loop to end and run() to return.
    */
    virtual void quit() = 0;

    virtual void openWindow(Window* window, const char* title, const WindowPosition& pos, int width, int height) = 0;
    virtual void closeWindow(Window* window) = 0;

    virtual void getWindowRenderSize(Window* window, int* width, int* height) = 0;
    virtual void getWindowSize(Window* window, int* width, int* height) = 0;

    virtual void setWindowPosition(Window* window, const WindowPosition& pos) = 0;
    virtual void setWindowSize(Window* window, int width, int height) = 0;
    virtual void setWindowTitle(Window* window, const char* title) = 0;

    virtual void setMenu(const Menu& menu) {}
    virtual void setWindowMenu(Window* window, const Menu& menu) {}

    virtual Window* activeWindow() = 0;

    /**
    * Should return a path suitable for storing preferences or other persistent user data.
    */
    virtual std::string userStoragePath() const = 0;

    /**
    * Override these if the platform supports a clipboard.
    */
    virtual void setClipboardText(const char* text) { _clipboard = text; }
    virtual std::string getClipboardText() { return _clipboard; }

    virtual void startTextInput() = 0;
    virtual void stopTextInput() = 0;

    /**
    * The default set of modifiers to apply to shortcuts.
    */
    virtual KeyModifier defaultShortcutModifier() const { return KeyModifier::kControl; }

    /**
    * Should return true if the platform has some sort of file browser or mechanism for selecting files.
    */
    virtual bool canSelectFiles() const { return false; }

    /**
    * Opens up the platform's file selection mechanism.
    *
    * @param allowFiles if false, non-directory files cannot be selected
    * @param allowDirectories if false, directories cannot be selected
    * @param allowMultiple if false, only a single file or directory can be selected
    * @param action the action to be taken once a selection is made
    */
    virtual void selectFiles(bool allowFiles,
                             bool allowDirectories,
                             bool allowMultiple,
                             std::function<void(std::vector<std::string>)> action) {
        ONAIR_ASSERT(false);
    }

    /**
    * Should return true if the platform can open the given URL.
    */
    virtual bool canOpenURL(const char* url) const { return false; }

    /**
    * Should open the given URL.
    */
    virtual bool openURL(const char* url) { return false; }

    /**
    * Opens up a dialog.
    *
    * @param action the action to be taken once a choice is made
    */
    virtual void openDialog(Window* window,
                            const char* title,
                            const char* message,
                            const std::vector<std::string>& buttons,
                            std::function<void(int)> action = std::function<void(int)>()) = 0;

    /**
    * Returns the name of the operating system.
    */
    virtual std::string operatingSystem() const { return "Unknown"; }

    /**
    * Enables or disables the screen saver.
    */
    virtual void setScreenSaverEnabled(bool enabled = true) {}

    /**
    * Returns path to resources, if there is one.
    */
    virtual std::string resourcePath() const { return ""; }

    virtual void setCursorType(CursorType type) {}

    virtual void showStatusBar() {}
    virtual void hideStatusBar() {}

protected:
    /**
    * _init() must be called from any final derived class' ctor in order to properly
    * set up the application menu and default ResourceManager.
    */
    void _init();
    void _update(Window* window) { window->_update(); }
    void _render(Window* window) { window->_render(); }
    void _didResize(Window* window, int width, int height) { window->_didResize(width, height); }
    void _assignWindowSize(Window* window) { getWindowSize(window, &window->_width, &window->_height); }

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
    std::unordered_map<std::string, std::shared_ptr<DownloadInfo>> _downloads;
    std::vector<std::future<void>> _backgroundTasks;
    TaskQueue _taskQueue;
    std::string _clipboard;
    std::unique_ptr<okui::FileResourceManager> _defaultResourceManager;
};

} // namespace okui
} // namespace onair
