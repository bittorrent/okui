/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include <okui/applications/OSX.h>

#if SCRAPS_MACOS

@implementation MenuTarget

- (id)initWithApplication:(okui::Application*)application {
    if (self = [super init]) {
        _application = application;
    }
    return self;
}

- (NSInteger)addMenuItem:(const okui::MenuItem*)item {
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
         case okui::MenuItem::State::kOff:   [(id)item setState: NSOffState]; break;
         case okui::MenuItem::State::kMixed: [(id)item setState: NSMixedState]; break;
         case okui::MenuItem::State::kOn:    [(id)item setState: NSOnState]; break;
         default: break;
     }

    return !command || _application->firstResponder()->chainCanHandleCommand(command);
}

- (const okui::MenuItem*)itemForObject:(id)object {
    return _menuItems[[object tag]];
}

@end

#endif
