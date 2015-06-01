#include "bittorrent/ui/platforms/OSX.h"

#if BT_MAC_OS_X

#include "bittorrent/ui/Application.h"

#import <AppKit/NSAlert.h>
#import <AppKit/NSOpenPanel.h>

@interface MenuTarget : NSObject {
    bittorrent::ui::Application* _application;
    std::vector<const bittorrent::ui::MenuItem*> _menuItems;
}
@end

@implementation MenuTarget

- (id)initWithApplication:(bittorrent::ui::Application*)application {
    if (self = [super init]) {
        _application = application;
    }
    return self;
}

- (NSInteger)addMenuItem:(const bittorrent::ui::MenuItem*)item {
    _menuItems.push_back(item);
    return _menuItems.size() - 1;
}

- (void)menuItemAction:(id)sender {
    auto item = [self itemForObject:sender];
    if (item->command()) {
        _application->firstResponder()->handleCommand(item->command(), item->commandContext());
    }
}

- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)item {
    auto command = [self itemForObject:item]->command();
    return !command || _application->firstResponder()->chainCanHandleCommand(command);
}

- (const bittorrent::ui::MenuItem*)itemForObject:(id)object {
    return _menuItems[[object tag]];
}

@end

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

void OSX::setApplicationMenu(Application* application, const Menu& menu) {
    _applicationMenuTarget = [[MenuTarget alloc] initWithApplication:application];
    _applicationMenu = menu;
    [NSApp setMainMenu:_convertMenu(_applicationMenu)];
}

void OSX::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    switch (key) {
        case KeyCode::kCapsLock:
        case KeyCode::kLControl:
        case KeyCode::kLShift:
        case KeyCode::kLAlt:
        case KeyCode::kLSuper:
        case KeyCode::kRControl:
        case KeyCode::kRShift:
        case KeyCode::kRAlt:
        case KeyCode::kRSuper:
        case KeyCode::kScrollLock:
            break;
        default:
            NSBeep();
    }
}

NSMenu* OSX::_convertMenu(const Menu& menu) {
    NSMenu* ret = [NSMenu new];
    for (auto& item : menu.items()) {
        [ret addItem:_convertMenuItem(item)];
    }
    return ret;
}

NSMenuItem* OSX::_convertMenuItem(const MenuItem& item) {
    if (item.isSeparator()) {
        return [NSMenuItem separatorItem];
    }
    unichar key = _convertKeyCode(item.keyCode());
    NSMenuItem* ret = [[NSMenuItem alloc] initWithTitle:[NSString stringWithUTF8String:item.label().c_str()] action:nil keyEquivalent:(key ? [NSString stringWithCharacters:&key length:1]: @"")];
    if (key) {
        ret.keyEquivalentModifierMask = _convertKeyModifiers(item.keyModifiers());
    }
    if (item.submenu()) {
        NSMenu* submenu = _convertMenu(item.submenu());
        submenu.title = [NSString stringWithUTF8String:item.label().c_str()];
        [ret setSubmenu:submenu];
    }
    ret.target = _applicationMenuTarget;
    ret.action = @selector(menuItemAction:);
    ret.tag = [_applicationMenuTarget addMenuItem:&item];
    return ret;
}

unichar OSX::_convertKeyCode(KeyCode keyCode) {
    if (static_cast<unichar>(keyCode) < 256) {
        return static_cast<unichar>(keyCode);
    }
    // TODO: convert non-ascii keys: https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSEvent_Class/#//apple_ref/doc/constant_group/Function_Key_Unicodes
    return 0;
}

NSUInteger OSX::_convertKeyModifiers(KeyModifiers modifiers) {
    NSUInteger ret = 0;
    if (modifiers & kShift) {
        ret |= NSShiftKeyMask;
    }
    if (modifiers & kAlt) {
        ret |= NSAlternateKeyMask;
    }
    if (modifiers & kControl) {
        ret |= NSControlKeyMask;
    }
    if (modifiers & kSuper) {
        ret |= NSCommandKeyMask;
    }
    return ret;
}

}}}

#endif
