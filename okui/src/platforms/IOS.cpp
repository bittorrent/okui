#include "onair/okui/platforms/IOS.h"

#if ONAIR_IOS

#import <UIKit/UIAlertView.h>
#import <UIKit/UIApplication.h>

@interface AlertViewHelper : NSObject<UIAlertViewDelegate>
    typedef void(^CompletionHandler)(NSInteger buttonIndex);
    @property (strong,nonatomic) CompletionHandler completionHandler;
    
    + (void)showAlertView:(UIAlertView*)alertView withCompletionHandler:(CompletionHandler)handler;
@end

@implementation AlertViewHelper
    + (void)showAlertView:(UIAlertView*)alertView withCompletionHandler:(CompletionHandler)handler {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Warc-retain-cycles"
        __block AlertViewHelper* delegate = [AlertViewHelper new];
        alertView.delegate = delegate;
        delegate.completionHandler = ^(NSInteger buttonIndex) {
            handler(buttonIndex);
            alertView.delegate = nil;
            delegate = nil;
        };
        [alertView show];
        #pragma clang diagnostic pop
    }
    
    - (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
        self.completionHandler(buttonIndex);
    }
@end

namespace onair {
namespace okui {
namespace platforms {

bool IOS::canOpenURL(const char* url) const {
    return [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

bool IOS::openURL(const char* url) {
    return [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

void IOS::openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action) {
    UIAlertView* alert = [UIAlertView new];
    alert.title = [NSString stringWithUTF8String:title];
    alert.message = [NSString stringWithUTF8String:message];
    for (auto& button : buttons) {
        [alert addButtonWithTitle:[NSString stringWithUTF8String:button.c_str()]];
    }
    [AlertViewHelper showAlertView:alert withCompletionHandler:^(NSInteger buttonIndex) {
        if (action) {
            action(buttonIndex);
        }
    }];
}

}}}

#endif
