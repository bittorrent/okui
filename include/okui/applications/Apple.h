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

#if SCRAPS_APPLE

#include <okui/applications/AppleUserPreferences.h>
#include <okui/FileResourceManager.h>
#include <okui/KeyCode.h>

#import <Foundation/Foundation.h>

namespace okui::applications {

/**
* Provides some native Application overrides for iOS and OS X.
*
* This isn't intended to be a full implementation, but can be used to add native support to other implementations.
*/
template <typename Base>
class Apple : public Base {
public:
    Apple();
    virtual ~Apple() {}

    virtual std::string userStoragePath() const override;

    virtual KeyModifier defaultShortcutModifier() const override { return KeyModifier::kSuper; }

    virtual UserPreferencesInterface* getUserPreferences() override { return _userPreferences.get(); }

private:
    std::unique_ptr<ResourceManager>      _resourceManager;
    std::unique_ptr<AppleUserPreferences> _userPreferences;
};

template <typename Base>
inline Apple<Base>::Apple() {
    NSString* path = [[NSBundle mainBundle] resourcePath];
    _resourceManager = std::make_unique<FileResourceManager>([path UTF8String]);
    Base::setResourceManager(_resourceManager.get());
    _userPreferences = std::make_unique<AppleUserPreferences>();
}

template <typename Base>
inline std::string Apple<Base>::userStoragePath() const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    return std::string{[applicationSupportDirectory UTF8String]} + '/' + this->organization() + '/' + this->name();
}

} // namespace okui::applications

#endif
