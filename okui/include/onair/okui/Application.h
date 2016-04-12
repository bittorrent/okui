#pragma once

#include "onair/okui/config.h"

#include "onair/okui/ResourceManager.h"
#include "onair/okui/Responder.h"
#include "onair/okui/Menu.h"
#include "onair/okui/Window.h"
#include "onair/okui/CursorTypes.h"

#include "onair/TaskQueue.h"

#if ONAIR_MAC_OS_X
#import <AppKit/NSWindow.h>
#endif

#if ONAIR_ANDROID
#include <jni.h>
#endif

#include <list>
#include <unordered_map>
#include <map>

namespace onair {
namespace okui {

class Application : public Responder {
public:
    virtual ~Application();

    virtual std::string name() const = 0;
    virtual std::string organization() const = 0;

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
    * Override this to set up the main run loop.
    */
    virtual void run() = 0;

    /**
    * A call to this method should cause the main loop to end and run() to return.
    */
    virtual void quit() = 0;

    /**
    * The following window functions should generally be avoided in favor of the more object-oriented Window class methods.
    */
    void addWindow(Window* window) { _windows.emplace_back(window); }
    void removeWindow(Window* window) { _windows.remove(window); }

    virtual void openWindow(Window* window, const char* title, const WindowPosition& pos, int width, int height) = 0;
    virtual void closeWindow(Window* window) = 0;

    virtual void getWindowRenderSize(Window* window, int* width, int* height) = 0;
    virtual void getWindowSize(Window* window, int* width, int* height) = 0;
    virtual void getWindowPosition(Window* window, int* x, int* y) = 0;

    virtual void setWindowPosition(Window* window, const WindowPosition& pos) = 0;
    virtual void setWindowSize(Window* window, int width, int height) = 0;
    virtual void setWindowTitle(Window* window, const char* title) = 0;

    virtual const Menu* getMenu() const { return nullptr; }
    virtual void setMenu(const Menu& menu) {}
    virtual void setWindowMenu(Window* window, const Menu& menu) {}

    virtual Window* activeWindow() = 0;
    virtual const std::list<Window*>& windows() const { return _windows; }

    ResourceManager* resourceManager() { return _resourceManager; }
    void setResourceManager(ResourceManager* resourceManager) { _resourceManager = resourceManager; }

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

    /**
    * Returns an id unique to this device.
    */
    virtual std::string distinctId() const { return ""; }

    /**
    * Returns the name of the operating system.
    */
    virtual std::string operatingSystem() const { return ""; }

    /**
    * Returns the name of the device model.
    */
    virtual std::string deviceModel() const  { return ""; }

    /**
    * Returns whether there is currently a network connection.
    */
    virtual bool hasNetworkConnection() const { return true; }

    /**
    * Returns whether the active network connection is a mobile connection.
    */
    virtual bool isMobileConnection() const { return false; }

    /**
    * Enables or disables the screen saver.
    */
    virtual void setScreenSaverEnabled(bool enabled = true) {}

    virtual void setCursorType(CursorType type) {}
    virtual void showCursor(bool visible = true) {}
    virtual bool isCursorVisible() const { return true; }

    virtual bool hasStatusBar() const { return false; }

    virtual void showStatusBar() {}
    virtual void hideStatusBar() {}

    /**
    * Queues up a tasks to be executed asynchronously (but on the main thread).
    *
    * This method is thread-safe.
    */
    TaskQueue* taskScheduler() { return &_taskQueue; }

    /**
    * The application-wide first responder is the active window's first responder or the application if
    * there are no open windows.
    */
    Responder* firstResponder() { return activeWindow() ? activeWindow()->firstResponder() : this; }

    /**
    * Executes the given command. Equivalent to `firstResponder()->handleCommand(command, context)`.
    */
    void command(Command command, CommandContext context) { firstResponder()->handleCommand(command, context); }

    virtual bool canHandleCommand(Command command) override {
        return command == kCommandQuit;
    }

    virtual void handleCommand(Command command, CommandContext context) override {
        if (command == kCommandQuit) {
            quit();
        }
    }

    std::shared_ptr<std::string> loadResource(const char* name) { return resourceManager()->load(name); }

    /**
    * Asynchronously downloads from the given URL.
    *
    * @param useCache if true, the results of a previously successful download may be provided
    */
    std::future<std::shared_ptr<const std::string>> download(const std::string& url, bool useCache = true);

    /**
    * Sets the CA bundle path to be used for secure requests made by the application.
    */
    void setCABundlePath(std::string caBundlePath) { _caBundlePath = std::move(caBundlePath); }

    /**
    * Returns the current CA bundle path.
    */
    const std::string& caBundlePath() const { return _caBundlePath; }

    /**
    * Used by views to post messages to listeners.
    */
    void post(View* sender, std::type_index index, const void* message, View::Relation relation);

    /**
    * Used by views to listed for posted messages.
    */
    void addListener(View* view, std::type_index index, std::function<void(const void*, View*)>* action, View::Relation relation);
    void removeListeners(View* view);

#if ONAIR_MAC_OS_X
    virtual NSWindow* nativeWindow(Window* window) const = 0;
#endif

#if ONAIR_ANDROID
    virtual jobject nativeActivity(JNIEnv** envOut = nullptr) const = 0;
#endif

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

    ResourceManager* _resourceManager;
    std::string _caBundlePath;
    std::unordered_map<std::string, std::shared_ptr<DownloadInfo>> _downloads;
    std::vector<std::future<void>> _backgroundTasks;
    std::list<Window*> _windows;

    std::string _clipboard;
    TaskQueue _taskQueue;

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
