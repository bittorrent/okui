#include "onair/okui/applications/IOS.h"

#if SCRAPS_IOS

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

#endif
