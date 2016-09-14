#pragma once

#include "okui/config.h"

#include "okui/CursorTypes.h"
#include "okui/DialogButton.h"
#include "okui/Menu.h"
#include "okui/Relation.h"
#include "okui/ResourceManager.h"
#include "okui/Responder.h"
#include "okui/UserPreferencesInterface.h"

#include "scraps/TaskQueue.h"

#if SCRAPS_MACOS
#import <AppKit/NSWindow.h>
#endif

#if SCRAPS_ANDROID
#include <jni.h>
#endif

#include <list>
#include <unordered_map>
#include <map>

namespace okui {

class Window;
struct WindowPosition;

class View;

class Application : public Responder {
public:
    Application();
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
    virtual void lowMemory() { purgeDownloadCache(0); }

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

    virtual bool isWindowMinimized(Window* window) const = 0;
    virtual void minimizeWindow(Window* window) = 0;

    virtual bool isWindowMaximized(Window* window) const = 0;
    virtual void maximizeWindow(Window* window) = 0;

    virtual bool isWindowFullscreen(Window* window) const = 0;
    virtual void fullscreenWindow(Window* window) = 0;

    virtual void restoreWindow(Window* window) = 0;

    virtual void bringWindowToFront(Window* window) = 0;
    virtual void bringAllWindowsToFront();

    virtual const Menu* getMenu() const { return nullptr; }
    virtual void setMenu(const Menu& menu) {}
    virtual void setWindowMenu(Window* window, const Menu& menu) {}

    virtual Window* activeWindow() = 0;
    virtual const std::list<Window*>& windows() const { return _windows; }

    ResourceManager* resourceManager() { return _resourceManager; }
    void setResourceManager(ResourceManager* resourceManager) { _resourceManager = resourceManager; }

    virtual UserPreferencesInterface* getUserPreferences() = 0;

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
        SCRAPS_ASSERT(false);
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
                            const std::vector<DialogButton>& buttons,
                            std::function<void(int)> action = {}) = 0;

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
    scraps::TaskQueue* taskScheduler() { return &_taskQueue; }

    /**
    * The application-wide first responder is the active window's first responder or the application if
    * there are no open windows.
    */
    Responder* firstResponder();

    /**
    * Executes the given command if the responder chain can handle it.
    */
    void command(Command command, CommandContext context = {});

    virtual bool canHandleCommand(Command command) override;

    virtual void handleCommand(Command command, CommandContext context) override;

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
    * Posts a message to all listeners subscribed to any, hierarchy, or ancestors.
    */
    template <typename T>
    void post(T& message) {
        _post(std::type_index(typeid(typename std::decay_t<T>)), &message);
    }

    template <typename T>
    void post(T&& message) {
        auto m = std::move(message);
        _post(std::type_index(typeid(typename std::decay_t<T>)), &m);
    }

    /**
    * Used by views to post messages to listeners.
    */
    void post(View* sender, std::type_index index, const void* message, Relation relation);

    /**
    * Used by views to listed for posted messages.
    */
    void addListener(View* view, std::type_index index, std::function<void(const void*, View*)>* action, Relation relation);
    void removeListeners(View* view);

#if SCRAPS_MACOS
    virtual NSWindow* nativeWindow(Window* window) const = 0;
#endif

#if SCRAPS_ANDROID
    virtual jobject nativeActivity(JNIEnv** envOut = nullptr) const = 0;
#endif

    size_t cachedDownloads() const { return _downloads.size(); }

    /**
    * @return the current size in bytes of the cache
    */
    size_t downloadCacheSize() const;

    /**
    * By default, downloads are cached, so that subsequent requests for the same url return a
    * cached result. The application will attempt to keep the cache below this size in bytes.
    */
    void setMaxDownloadCacheSize(size_t size) { _maxDownloadCacheSize = size; }

    /**
    * Purges the least recently used items from the download cache until the size is under maxSize
    * in bytes.
    */
    void purgeDownloadCache(size_t maxSize);

    /**
    * Makes the given object available via get(). This can be used to store or make available arbitrary
    * state that views can access via View::inherit.
    */
    template <typename T>
    auto set(T&& object);

    /**
    * Returns a pointer to an object stored via set.
    */
    template <typename T>
    T* get();

protected:
    void _update(Window* window);
    void _render(Window* window);
    void _didResize(Window* window, int width, int height);
    void _assignWindowSize(Window* window);

private:
    struct DownloadInfo {
        size_t size() const {
            auto r = result;
            return sizeof(*this) + (r ? r->size() : 0);
        }

        std::mutex mutex;
        std::shared_ptr<const std::string> result = nullptr;
        int inProgress                            = 0;
        std::vector<std::promise<std::shared_ptr<const std::string>>> promises;
    };

    struct Provision {
        Provision(stdts::any object) : object{std::move(object)} {}
        stdts::any object;
    };

    struct Listener {
        Listener(View* view, std::function<void(const void*, View*)>* action, Relation relation)
            : view{view}, action{action}, relation{relation} {}

        View* view;
        std::function<void(const void*, View*)>* action;
        Relation relation;
    };

    void _post(std::type_index index, const void* message);

    std::list<Window*>                          _windows;
    ResourceManager*                            _resourceManager;
    std::string                                 _caBundlePath;
    std::string                                 _clipboard;

    std::multimap<std::type_index, Listener>    _listeners;
    std::list<Provision>                        _provisions;

    std::vector<std::future<void>>              _backgroundTasks;
    scraps::TaskQueue                           _taskQueue;
    std::unordered_map<std::string, std::shared_ptr<DownloadInfo>> _downloads;
    std::list<decltype(_downloads.begin())> _downloadUseOrder;

    size_t _maxDownloadCacheSize = 100 * 1024 * 1024;
};

template <typename T>
auto Application::set(T&& object) {
    _provisions.emplace_front(std::forward<T>(object));
    return stdts::any_cast<std::decay_t<T>>(&_provisions.front().object);
}

/**
* Returns a pointer to an object stored via set.
*/
template <typename T>
T* Application::get() {
    for (auto& provision : _provisions) {
        if (auto object = stdts::any_cast<T>(&provision.object)) {
            return object;
        } else if (auto pointer = stdts::any_cast<T*>(&provision.object)) {
            return *pointer;
        }
    }
    return nullptr;
}

} // namespace okui
