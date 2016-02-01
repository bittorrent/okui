#pragma once

#include "onair/okui/config.h"

#if ONAIR_MAC_OS_X

#include "onair/okui/applications/Apple.h"
#include "onair/okui/Application.h"

#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CFString.h>

#import <AppKit/AppKit.h>
#import <AppKit/NSAlert.h>
#import <AppKit/NSMenu.h>
#import <AppKit/NSOpenPanel.h>

@interface MenuTarget : NSObject {    
    onair::okui::Application* _application;
    std::vector<const onair::okui::MenuItem*> _menuItems;
}
- (id)initWithApplication:(onair::okui::Application*)application;
- (NSInteger)addMenuItem:(const onair::okui::MenuItem*)item;
- (void)menuItemAction:(id)sender;
- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)item;
@end

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Platform overrides for OS X.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
template <typename Base>
class OSX : public Apple<Base> {
public:
    OSX();
    virtual ~OSX();

    virtual bool canSelectFiles() const override { return true; }
    virtual void selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) override;

    virtual bool canOpenURL(const char* url) const override;
    virtual bool openURL(const char* url) override;

    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action = std::function<void(int)>()) override;

    virtual const Menu* getMenu() const override { return &_applicationMenu; }
    virtual void setMenu(const Menu& menu) override;

    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

    virtual std::string distinctId() const override;

private:
    NSMenu* _convertMenu(const Menu& menu);
    NSMenuItem* _convertMenuItem(const MenuItem& item);
    unichar _convertKeyCode(KeyCode keyCode);
    NSUInteger _convertKeyModifiers(KeyModifiers modifiers);

    id _applicationMenuTarget = nil;
    Menu _applicationMenu;
};

template <typename Base>
inline OSX<Base>::OSX() {
    Base::setMenu(Menu({
        MenuItem("Edit", Menu({
            MenuItem("Copy", kCommandCopy, KeyCode::kC, this->defaultShortcutModifier()),
            MenuItem("Cut", kCommandCut, KeyCode::kX, this->defaultShortcutModifier()),
            MenuItem("Paste", kCommandPaste, KeyCode::kV, this->defaultShortcutModifier()),
            MenuItem("Select All", kCommandSelectAll, KeyCode::kA, this->defaultShortcutModifier()),
        })),
    }));
}

template <typename Base>
inline OSX<Base>::~OSX() {
    [NSApp setMainMenu:nil];
}

template <typename Base>
inline void OSX<Base>::selectFiles(bool allowFiles, bool allowDirectories, bool allowMultiple, std::function<void(std::vector<std::string>)> action) {
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

template <typename Base>
inline bool OSX<Base>::canOpenURL(const char* url) const {
    return [[NSWorkspace sharedWorkspace] URLForApplicationToOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

template <typename Base>
inline bool OSX<Base>::openURL(const char* url) {
    return [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

template <typename Base>
inline void OSX<Base>::openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action) {
    NSAlert* alert = [NSAlert new];
    alert.messageText = [NSString stringWithUTF8String:title];
    alert.informativeText = [NSString stringWithUTF8String:message];
    for (auto& button : buttons) {
        [alert addButtonWithTitle:[NSString stringWithUTF8String:button.c_str()]];
    }
    [alert beginSheetModalForWindow:(window ? Base::nativeWindow(window) : [NSApp keyWindow]) completionHandler:^(NSModalResponse returnCode) {
        if (action) {
            action(returnCode - NSAlertFirstButtonReturn);
        }
    }];
}

template <typename Base>
inline void OSX<Base>::setMenu(const Menu& menu) {
    _applicationMenuTarget = [[MenuTarget alloc] initWithApplication:this];
    _applicationMenu = menu;
    NSMenu* mainMenu = _convertMenu(_applicationMenu);

    // insert a standard "apple" menu (which is oddly the menu to the right of the menu with the apple icon)

    NSMenu* appleMenu = [NSMenu new];

    NSString* appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleDisplayName"];
    if (!appName) {
        appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
    }
    if (!appName) {
        appName = @"Application";
    }

    [appleMenu addItemWithTitle:[@"About " stringByAppendingString:appName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
    [appleMenu addItem:[NSMenuItem separatorItem]];
    NSMenu* servicesMenu = [NSMenu new];
    NSMenuItem* menuItem = [appleMenu addItemWithTitle:@"Services" action:nil keyEquivalent:@""];
    [menuItem setSubmenu:servicesMenu];
    [NSApp setServicesMenu:servicesMenu];
    [appleMenu addItem:[NSMenuItem separatorItem]];
    [appleMenu addItemWithTitle:[@"Hide " stringByAppendingString:appName] action:@selector(hide:) keyEquivalent:@"h"];
    menuItem = (NSMenuItem*)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
    [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];
    [appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
    [appleMenu addItem:[NSMenuItem separatorItem]];
    [appleMenu addItemWithTitle:[@"Quit " stringByAppendingString:appName] action:@selector(terminate:) keyEquivalent:@"q"];

    menuItem = [NSMenuItem new];
    [menuItem setSubmenu:appleMenu];
    [mainMenu insertItem:menuItem atIndex:0];

    [NSApp setMainMenu:mainMenu];
}

template <typename Base>
inline void OSX<Base>::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
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

template <typename Base>
inline std::string OSX<Base>::distinctId() const {
    io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/");
    CFStringRef uuidCf = (CFStringRef) IORegistryEntryCreateCFProperty(ioRegistryRoot, CFSTR(kIOPlatformUUIDKey), kCFAllocatorDefault, 0);
    IOObjectRelease(ioRegistryRoot);
    char distinctId[128] = {};
    CFStringGetCString(uuidCf, distinctId, 128, kCFStringEncodingUTF8);
    CFRelease(uuidCf);
    return distinctId;
}

template <typename Base>
inline NSMenu* OSX<Base>::_convertMenu(const Menu& menu) {
    NSMenu* ret = [NSMenu new];
    for (auto& item : menu.items()) {
        [ret addItem:_convertMenuItem(item)];
    }
    return ret;
}

template <typename Base>
inline NSMenuItem* OSX<Base>::_convertMenuItem(const MenuItem& item) {
    if (item.isSeparator()) {
        return [NSMenuItem separatorItem];
    }
    unichar key = _convertKeyCode(item.keyCode());
    NSMenuItem* ret = [[NSMenuItem alloc] initWithTitle:[NSString stringWithUTF8String:item.label().c_str()] action:nil keyEquivalent:(key ? [NSString stringWithCharacters:&key length:1]: @"")];
    if (key) {
        ret.keyEquivalentModifierMask = _convertKeyModifiers(item.keyModifiers());
    }
    if (!item.submenu().isEmpty()) {
        NSMenu* submenu = _convertMenu(item.submenu());
        submenu.title = [NSString stringWithUTF8String:item.label().c_str()];
        [ret setSubmenu:submenu];
    }
    ret.target = _applicationMenuTarget;
    ret.action = @selector(menuItemAction:);
    ret.tag = [_applicationMenuTarget addMenuItem:&item];

    return ret;
}

template <typename Base>
inline unichar OSX<Base>::_convertKeyCode(KeyCode keyCode) {
    if (static_cast<unichar>(keyCode) < 256) {
        return static_cast<unichar>(keyCode);
    }

    // https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSEvent_Class/#//apple_ref/doc/constant_group/Function_Key_Unicodes
    switch (keyCode) {
        case KeyCode::kUp:    return NSUpArrowFunctionKey;
        case KeyCode::kDown:  return NSDownArrowFunctionKey;
        case KeyCode::kLeft:  return NSLeftArrowFunctionKey;
        case KeyCode::kRight: return NSRightArrowFunctionKey;
        default:              return 0;
    };
}

template <typename Base>
inline NSUInteger OSX<Base>::_convertKeyModifiers(KeyModifiers modifiers) {
    NSUInteger ret = 0;
    if (modifiers & KeyModifier::kShift) {
        ret |= NSShiftKeyMask;
    }
    if (modifiers & KeyModifier::kAlt) {
        ret |= NSAlternateKeyMask;
    }
    if (modifiers & KeyModifier::kControl) {
        ret |= NSControlKeyMask;
    }
    if (modifiers & KeyModifier::kSuper) {
        ret |= NSCommandKeyMask;
    }
    return ret;
}

}}}

#endif
