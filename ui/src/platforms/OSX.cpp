#include "bittorrent/ui/platforms/OSX.h"

#if BT_MAC_OS_X

#import <AppKit/NSAlert.h>
#import <AppKit/NSOpenPanel.h>

namespace bittorrent {
namespace ui {
namespace platforms {

void OSX::selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) {
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

bool OSX::canOpenURL(const char* url) const {
    return [[NSWorkspace sharedWorkspace] URLForApplicationToOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

bool OSX::openURL(const char* url) {
    return [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

void OSX::openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action) {
    NSAlert* alert = [NSAlert new];
    alert.messageText = [NSString stringWithUTF8String:title];
    alert.informativeText = [NSString stringWithUTF8String:message];
    for (auto& button : buttons) {
        [alert addButtonWithTitle:[NSString stringWithUTF8String:button.c_str()]];
    }
    [alert beginSheetModalForWindow:(window ? nativeWindow(window) : [NSApp keyWindow]) completionHandler:^(NSModalResponse returnCode) {
        if (action) {
            action(returnCode - NSAlertFirstButtonReturn);
        }
    }];
}

}}}

#endif
