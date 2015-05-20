#pragma once

#include "bittorrent/ui/config.h"

#if BT_MAC_OS_X

#include "bittorrent/ui/platforms/Apple.h"

#import <AppKit/NSWindow.h>

namespace bittorrent {
namespace ui {
namespace platforms {

/**
* Provides some native Platform overrides for OS X.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class OSX : public Apple {
public:
    virtual NSWindow* nativeWindow(Window* window) const = 0;
    
    virtual bool canSelectFiles() const override { return true; }
    virtual void selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) override;

    virtual bool canOpenURL(const char* url) const override;
    virtual bool openURL(const char* url) override;

    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action = std::function<void(int)>()) override;
};

}}}

#endif