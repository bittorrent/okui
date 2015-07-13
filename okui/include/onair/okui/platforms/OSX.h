#pragma once

#include "onair/okui/config.h"

#if ONAIR_MAC_OS_X

#include "onair/okui/platforms/Apple.h"

#import <AppKit/NSMenu.h>
#import <AppKit/NSWindow.h>

namespace onair {
namespace okui {
namespace platforms {

/**
* Provides some native Platform overrides for OS X.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class OSX : public Apple {
public:
    virtual ~OSX();

    virtual NSWindow* nativeWindow(Window* window) const = 0;
    
    virtual bool canSelectFiles() const override { return true; }
    virtual void selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) override;

    virtual bool canOpenURL(const char* url) const override;
    virtual bool openURL(const char* url) override;

    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action = std::function<void(int)>()) override;

    virtual void setApplicationMenu(Application* application, const Menu& menu) override;

    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

private:
    NSMenu* _convertMenu(const Menu& menu);
    NSMenuItem* _convertMenuItem(const MenuItem& item);
    unichar _convertKeyCode(KeyCode keyCode);
    NSUInteger _convertKeyModifiers(KeyModifiers modifiers);

    id _applicationMenuTarget = nil;
    Menu _applicationMenu;    
};

}}}

#endif
