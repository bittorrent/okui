#pragma once

#include "bittorrent/ui/config.h"

#if BT_MAC_OS_X

#include "bittorrent/ui/Platform.h"

#import <AppKit/NSOpenPanel.h>

namespace bittorrent {
namespace ui {
namespace platforms {

/**
* Provides some native Platform overrides for Mac OS X.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class MacOSX : public Platform {
public:
    virtual std::string userStoragePath(const char* application, const char* organization) const override;

    virtual KeyModifier defaultShortcutModifier() override { return KeyModifier::kSuper; }
    
    virtual bool canSelectFiles() const override { return true; }
    virtual void selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) override;
};

}}}

#endif