#include "onair/okui/applications/OSX.h"

#if ONAIR_MAC_OS_X

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
    auto menuItem = [self itemForObject:item];
    auto command = menuItem->command();

     switch (menuItem->state()) {
         case onair::okui::MenuItem::State::kOff:   [(id)item setState: NSOffState]; break;
         case onair::okui::MenuItem::State::kMixed: [(id)item setState: NSMixedState]; break;
         case onair::okui::MenuItem::State::kOn:    [(id)item setState: NSOnState]; break;
         default: break;
     }

    return !command || _application->firstResponder()->chainCanHandleCommand(command);
}

- (const onair::okui::MenuItem*)itemForObject:(id)object {
    return _menuItems[[object tag]];
}

@end

#endif
