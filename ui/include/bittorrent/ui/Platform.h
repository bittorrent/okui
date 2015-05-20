#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Window.h"

namespace bittorrent {
namespace ui {

/**
* This class abstracts all of the platform-specific functionality of an application.
*
* Implement this to add support for a new platform.
*/
class Platform {
public:
    virtual ~Platform() {}

    /**
    * Queues up a task to be executed asynchronously (but on the main thread).
    *
    * This method is thread-safe.
    */
    void async(std::function<void()> task);
    
    /**
    * Carries out work such as driving asynchronous operations.
    */
    void work();

    /**
    * Override this to set up the main run loop. The run loop must regularly call work().
    */
    virtual void run() = 0;
    
    /**
    * A call to this method should cause the main loop to end and run() to return.
    */
    virtual void quit() = 0;

    virtual void openWindow(Window* window, const char* title, int x, int y, int width, int height) = 0;
    virtual void closeWindow(Window* window) = 0;

    virtual void getWindowRenderSize(Window* window, int* width, int* height) = 0;

    virtual void setWindowPosition(Window* window, int x, int y) = 0;
    virtual void setWindowSize(Window* window, int width, int height) = 0;
    virtual void setWindowTitle(Window* window, const char* title) = 0;

    /**
    * Should return a path suitable for storing preferences or other persistent user data.
    */
    virtual std::string userStoragePath(const char* application, const char* organization) const = 0;

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
    virtual KeyModifier defaultShortcutModifier() const { return KeyModifier::kCtrl; }
    
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
    virtual void selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) { BT_ASSERT(false); }

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
    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action = std::function<void(int)>()) = 0;

    /**
    * Returns the name of the operating system.
    */
    virtual std::string operatingSystem() const { return "Unknown"; }

    /**
    * Enables or disables the screen saver.
    */
    virtual void setScreenSaverEnabled(bool enabled = true) {}

protected:
    void _render(Window* window) { window->_render(); }
    void _didResize(Window* window, int width, int height) { window->_didResize(width, height); }

private:
    std::mutex _asyncMutex;
    std::vector<std::function<void()>> _asyncTasks;
    
    std::string _clipboard;
};

}}
