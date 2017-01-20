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
#include <okui/config.h>

#include <okui/Application.h>

#if SCRAPS_MACOS

#import <AppKit/AppKit.h>

@interface OKUISDLApplication : NSApplication
@property okui::Application* application;
@end

@implementation OKUISDLApplication
// selectively give events to sdl
- (NSEvent*)nextEventMatchingMask:(NSUInteger)mask untilDate:(NSDate*)expiration inMode:(NSString*)mode dequeue:(BOOL)flag {
    NSEvent* event = [super nextEventMatchingMask:mask untilDate:expiration inMode:mode dequeue:flag];

    if (event.type != NSKeyDown && event.type != NSKeyUp) {
        return event;
    }

    if ((self.keyWindow && [self.keyWindow performKeyEquivalent:event]) || [self.mainMenu performKeyEquivalent:event]) {
        // if this is a key equivalent, let the owner handle it instead of sdl
        return flag ? [self nextEventMatchingMask:mask untilDate:expiration inMode:mode dequeue:flag] : nil;
    }

    return event;
}

- (void)terminate:(id)sender {
    _application->quit();
}

@end

#endif
