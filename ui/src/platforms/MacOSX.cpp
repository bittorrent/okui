#include "bittorrent/ui/platforms/MacOSX.h"

#if BT_MAC_OS_X

namespace bittorrent {
namespace ui {
namespace platforms {

std::string MacOSX::userStoragePath(const char* application, const char* organization) const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    return std::string([applicationSupportDirectory UTF8String]) + '/' + organization + '/' + application;
}

void MacOSX::selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) {
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel setLevel:CGShieldingWindowLevel()];
    [panel setCanChooseFiles:allowFiles];
    [panel setCanChooseDirectories:allowDirectories];
    [panel setAllowsMultipleSelection:allowMultiple];
    [panel beginWithCompletionHandler:^(NSInteger result) {
        if (result == NSFileHandlingPanelCancelButton) {
            action(std::vector<std::string>());
            return;
        }
        
        std::vector<std::string> selection;
        for (NSString* filename in panel.filenames) {
            selection.push_back([filename UTF8String]);
        }
        action(std::move(selection));
    }];
    [panel makeKeyAndOrderFront:nil];
}

}}}

#endif
