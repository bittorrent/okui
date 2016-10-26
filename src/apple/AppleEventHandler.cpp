#include "okui/apple/AppleEventHandler.h"

#if SCRAPS_MACOS

#import <Foundation/Foundation.h>

@implementation AppleEventHandler

- (void)handleAppleEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent {
    self.handler(event, replyEvent);
}
@end
#endif
