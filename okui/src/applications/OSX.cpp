#include "onair/okui/applications/OSX.h"

#if ONAIR_MAC_OS_X

#include "onair/okui/Application.h"

#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CFString.h>

#import <AppKit/NSAlert.h>
#import <AppKit/NSOpenPanel.h>

@interface MenuTarget : NSObject {
    onair::okui::Application* _application;
    std::vector<const onair::okui::MenuItem*> _menuItems;
}
@end

@implementation MenuTarget

- (id)initWithApplication:(onair::okui::Application*)application {
    if (self = [super init]) {
        _application = application;
    }
    return self;
}

- (NSInteger)addMenuItem:(const onair::okui::MenuItem*)item {
    _menuItems.push_back(item);
    return _menuItems.size() - 1;
}

- (void)menuItemAction:(id)sender {
    auto item = [self itemForObject:sender];
    if (item->command()) {
        _application->command(item->command(), item->commandContext());
    }
}

- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)item {
    auto command = [self itemForObject:item]->command();
    return !command || _application->firstResponder()->chainCanHandleCommand(command);
}

- (const onair::okui::MenuItem*)itemForObject:(id)object {
    return _menuItems[[object tag]];
}

@end

namespace onair {
namespace okui {
namespace applications {

OSX::~OSX() {
    [NSApp setMainMenu:nil];
}

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

void OSX::setMenu(const Menu& menu) {
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

std::string OSX::distinctId() const {
    io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/");
    CFStringRef uuidCf = (CFStringRef) IORegistryEntryCreateCFProperty(ioRegistryRoot, CFSTR(kIOPlatformUUIDKey), kCFAllocatorDefault, 0);
    IOObjectRelease(ioRegistryRoot);
    char distinctId[128] = {};
    CFStringGetCString(uuidCf, distinctId, 128, kCFStringEncodingUTF8);
    CFRelease(uuidCf);
    return distinctId;
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
    if (!item.submenu().isEmpty()) {
        NSMenu* submenu = _convertMenu(item.submenu());
        submenu.title = [NSString stringWithUTF8String:item.label().c_str()];
        [ret setSubmenu:submenu];
    }
    ret.target = _applicationMenuTarget;
    ret.action = @selector(menuItemAction:);
    ret.tag = [_applicationMenuTarget addMenuItem:&item];

    switch (item.state()) {
        case MenuItem::State::kOff:   [ret setState: 0]; break;
        case MenuItem::State::kMixed: [ret setState: -1]; break;
        case MenuItem::State::kOn:    [ret setState: 1]; break;
        default: break;
    }

    return ret;
}

unichar OSX::_convertKeyCode(KeyCode keyCode) {
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

NSUInteger OSX::_convertKeyModifiers(KeyModifiers modifiers) {
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
