#pragma once

#include <okui/config.h>

#if SCRAPS_MACOS
#import <objc/NSObject.h>

#include <functional>

@class NSAppleEventDescriptor;

@interface AppleEventHandler : NSObject

@property std::function<void(NSAppleEventDescriptor* event, NSAppleEventDescriptor* replyEvent)> handler;

- (void)handleAppleEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent;
@end
#endif
