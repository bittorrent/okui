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
