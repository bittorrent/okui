#pragma once

#include "onair/okui/config.h"

#if SCRAPS_APPLE

#include "onair/okui/applications/AppleUserPreferences.h"
#include "onair/okui/FileResourceManager.h"
#include "onair/okui/KeyCode.h"

#import <Foundation/Foundation.h>

namespace onair {
namespace okui {
namespace applications {

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

}}}

#endif
