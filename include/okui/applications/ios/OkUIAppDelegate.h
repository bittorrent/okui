#pragma once

#include <okui/config.h>

#include <scraps/platform.h>

#if SCRAPS_IOS

#include <string>
#include <functional>

#import <private-headers/SDL_uikitappdelegate.h>

@interface SDLUIKitDelegate (MethodSwizzle)
+ (void)load;
+ (NSString*)myGetAppDelegateClassName; // will replace getAppDelegateClassName
@end

@interface OkUIAppDelegate : SDLUIKitDelegate
- (BOOL)application:(UIApplication*)app openURL:(NSURL*)url options:(NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options;
@end

namespace okui::applications::ios {

void SetAppDelegateClassName(std::string name);

} // namespace okui::applications::iOS

#endif
