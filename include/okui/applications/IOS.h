#pragma once

#include <okui/config.h>

#if SCRAPS_IOS || SCRAPS_TVOS

#include <okui/applications/Apple.h>
#include <okui/Window.h>

#import <UIKit/UIAlertView.h>
#import <UIKit/UIApplication.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#include <sys/utsname.h>

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

    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<DialogButton>& buttons, std::function<void(int)> action = {}) override;

    virtual bool hasStatusBar() const override { return true; }
    virtual void showStatusBar() override;
    virtual void hideStatusBar() override;

    virtual std::string operatingSystem() const override;
    virtual std::string deviceModel() const override;
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
inline void IOS<Base>::openDialog(Window* window, const char* title, const char* message, const std::vector<DialogButton>& buttons, std::function<void(int)> action) {
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:[NSString stringWithUTF8String:title]
                                message:[NSString stringWithUTF8String:message] preferredStyle:UIAlertControllerStyleAlert];

    int i = 0;
    for (auto& button : buttons) {
        auto style = button.isCancel ? UIAlertActionStyleCancel : (button.isDestructive ? UIAlertActionStyleDestructive : UIAlertActionStyleDefault);
        UIAlertAction* alertAction = [UIAlertAction actionWithTitle:[NSString stringWithUTF8String:button.label.c_str()] style:style handler:^(UIAlertAction* alertAction) {
            action(i);
        }];
        [alert addAction:alertAction];
        ++i;
    }

    [[UIApplication sharedApplication].keyWindow.rootViewController presentViewController:alert animated:YES completion:nil];
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
inline std::string IOS<Base>::deviceModel() const {
    utsname systemInfo;
    uname(&systemInfo);
    return systemInfo.machine;
}

template <typename Base>
inline std::string IOS<Base>::distinctId() const {
    auto* uuid = [UIDevice currentDevice].identifierForVendor.UUIDString;
    return std::string([uuid UTF8String]);
}

} } // namespace okui::applications

#endif
