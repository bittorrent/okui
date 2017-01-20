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
#include <okui/applications/ios/OkUIAppDelegate.h>

#include <okui/Application.h>

#if SCRAPS_IOS

#include <scraps/logging.h>
#include <scraps/URL.h>

#import <objc/runtime.h>
#import <objc/message.h>

namespace okui::applications::ios {

namespace detail {
    NSString* gAppDelegateClassName = @"OkUIAppDelegate";
}

void SetAppDelegateClassName(std::string name) {
    detail::gAppDelegateClassName = [NSString stringWithUTF8String: name.c_str()];
}

} // namespace okui::applications::iOS

@implementation SDLUIKitDelegate (MethodSwizzle)
// In order to avoid the warning "category is implementing a method which will also be implemented by its primary
// class" by duplicating method implementations, we'll add a new method and then swap the old with the new at
// runtime.
// from http://nshipster.com/method-swizzling/
+ (void)load {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        SEL originalSelector = @selector(getAppDelegateClassName);
        SEL swizzledSelector = @selector(myGetAppDelegateClassName);

        Class cls = object_getClass((id)self);

        Method originalMethod = class_getClassMethod(cls, originalSelector);
        Method swizzledMethod = class_getClassMethod(cls, swizzledSelector);

        BOOL didAddMethod = class_addMethod(cls, originalSelector, method_getImplementation(swizzledMethod), method_getTypeEncoding(swizzledMethod));

        if (didAddMethod) {
            class_replaceMethod(cls, swizzledSelector, method_getImplementation(originalMethod), method_getTypeEncoding(originalMethod));
        } else {
            method_exchangeImplementations(originalMethod, swizzledMethod);
        }
    });
}

+ (NSString*)myGetAppDelegateClassName {
    return okui::applications::ios::detail::gAppDelegateClassName;
}
@end

@implementation OkUIAppDelegate

- (BOOL)application:(UIApplication*)app openURL:(NSURL*)url options:(NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options {
    auto* defaultApplication = okui::DefaultApplication();
    if (defaultApplication) {
        defaultApplication->handleURL([[url absoluteString] UTF8String]);
        return YES;
    }
    return NO;
}

@end

#endif
