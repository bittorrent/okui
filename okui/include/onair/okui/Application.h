#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Menu.h"
#include "onair/okui/ResourceManager.h"
#include "onair/okui/Responder.h"
#include "onair/okui/Menu.h"
#include "onair/okui/Window.h"
#include "onair/okui/CursorTypes.h"

#include "onair/TaskQueue.h"

#include <unordered_map>
#include <thread>
#include <typeindex>
#include <map>

namespace onair {
namespace okui {

class Application : public Responder {
public:
    /**
    * @param name human readable application name. e.g. "Example Application"
    * @param organization human readable organization name. e.g. "My Organization"
    * @param resourceManager the resource manager. this is typically provided by subclass implementations
    * @param shouldInitialize if true, the application is ready for use immediately after construction. otherwise
    *                         initialize() must be called afterwards
    */
    Application(std::string name, std::string organization, ResourceManager* resourceManager = nullptr, bool shouldInitialize = true);
    virtual ~Application();

    /**
    * Initializes the application. This must be called before it is ready to be used if the constructor's
    * shouldInitialize parameter is false.
    */
    virtual void initialize();

    ResourceManager* resourceManager() { return _resourceManager; }
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
    * Sets the CA bundle path to be used for secure requests made by the application.
    */
    void setCABundlePath(std::string caBundlePath) { _caBundlePath = std::move(caBundlePath); }

    /**
    * Returns the current CA bundle path.
    */
    const std::string& caBundlePath() const { return _caBundlePath; }

    /**
    * Asynchronously downloads from the given URL.
    *
    * @param useCache if true, the results of a previously successful download may be provided
    */
    std::future<std::shared_ptr<const std::string>> download(const std::string& url, bool useCache = true);

    // Responder overrides
    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

    /**
    * iOS: applicationWillTerminate(), Android: onDestroy()
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
    * Override this to set up the main run loop.
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

    virtual double renderScale() const { return 1.0; }

    virtual std::string distinctId() const { return ""; }

    /**
    * Returns the name of the operating system.
    */
    virtual std::string operatingSystem() const { return "Unknown"; }

    /**
    * Enables or disables the screen saver.
    */
    virtual void setScreenSaverEnabled(bool enabled = true) {}

    virtual void setCursorType(CursorType type) {}

    virtual void showStatusBar() {}
    virtual void hideStatusBar() {}

    /**
    * Used by views to post messages to listeners.
    */
    void post(View* sender, std::type_index index, const void* message, View::Relation relation);

    /**
    * Used by views to listed for posted messages.
    */
    void addListener(View* view, std::type_index index, std::function<void(const void*, View*)>* action, View::Relation relation);
    void removeListeners(View* view);

protected:
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
    std::string _caBundlePath;
    std::unordered_map<std::string, std::shared_ptr<DownloadInfo>> _downloads;
    std::vector<std::future<void>> _backgroundTasks;
    TaskQueue _taskQueue;
    std::string _clipboard;

    struct Listener {
        Listener(View* view, std::function<void(const void*, View*)>* action, View::Relation relation)
            : view{view}, action{action}, relation{relation} {}

        View* view;
        std::function<void(const void*, View*)>* action;
        View::Relation relation;
    };

    std::multimap<std::type_index, Listener> _listeners;
};

} // namespace okui
} // namespace onair
