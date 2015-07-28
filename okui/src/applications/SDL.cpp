#include "onair/okui/config.h"

#include "onair/okui/Application.h"

#if ONAIR_MAC_OS_X

#import <AppKit/AppKit.h>

@interface OKUISDLApplication : NSApplication
@property onair::okui::Application* app;
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
    _app->quit();
}

@end

#endif
