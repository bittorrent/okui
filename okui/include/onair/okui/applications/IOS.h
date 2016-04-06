#pragma once

#include "onair/okui/config.h"

#if SCRAPS_IOS || SCRAPS_TVOS

#include "onair/okui/applications/Apple.h"
#include "onair/okui/Window.h"

#import <UIKit/UIAlertView.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#if SCRAPS_IOS
@interface AlertViewHelper : NSObject<UIAlertViewDelegate>
    typedef void(^CompletionHandler)(NSInteger buttonIndex);
    @property (strong,nonatomic) CompletionHandler completionHandler;

    + (void)showAlertView:(UIAlertView*)alertView withCompletionHandler:(CompletionHandler)handler;
    - (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex;
@end
#endif

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Platform overrides for iOS.
*
* This isn't intended to be a full implementation, but can be used to add native support to other implementations.
*/
template <typename Base>
class IOS : public Apple<Base> {
public:
    virtual bool canOpenURL(const char* url) const override;
    virtual bool openURL(const char* url) override;

    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action = std::function<void(int)>()) override;

    virtual bool hasStatusBar() const override { return true; }
    virtual void showStatusBar() override;
    virtual void hideStatusBar() override;

    virtual std::string operatingSystem() const override;
    virtual std::string distinctId() const override;
};

template <typename Base>
inline bool IOS<Base>::canOpenURL(const char* url) const {
    return [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

template <typename Base>
inline bool IOS<Base>::openURL(const char* url) {
    return [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

template <typename Base>
inline void IOS<Base>::openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action) {
#if SCRAPS_IOS
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
#endif
}

template <typename Base>
inline void IOS<Base>::showStatusBar() {
#if SCRAPS_IOS
    [[UIApplication sharedApplication] setStatusBarHidden:false withAnimation:UIStatusBarAnimationNone];
#endif
}

template <typename Base>
inline void IOS<Base>::hideStatusBar() {
#if SCRAPS_IOS
    [[UIApplication sharedApplication] setStatusBarHidden:true withAnimation:UIStatusBarAnimationNone];
#endif
}

template <typename Base>
inline std::string IOS<Base>::operatingSystem() const {
    UIDevice* device = [UIDevice currentDevice];
    return scraps::Format("{} {}", [[device systemName] UTF8String], [[device systemVersion] UTF8String]);
}

template <typename Base>
inline std::string IOS<Base>::distinctId() const {
    auto* uuid = [UIDevice currentDevice].identifierForVendor.UUIDString;
    return std::string([uuid UTF8String]);
}

} // namespace applications
} // namespace okui
} // namespace onair

#endif
