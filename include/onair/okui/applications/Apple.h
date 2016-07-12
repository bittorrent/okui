#pragma once

#include "onair/okui/config.h"

#if __APPLE__

#include "onair/okui/FileResourceManager.h"
#include "onair/okui/KeyCode.h"

#import <Foundation/Foundation.h>

#include <sys/utsname.h>

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

    virtual std::string deviceModel() const override;

private:
    std::unique_ptr<ResourceManager> _resourceManager;
};

template <typename Base>
inline Apple<Base>::Apple() {
    	NSString* path = [[NSBundle mainBundle] resourcePath];
    _resourceManager = std::make_unique<FileResourceManager>([path UTF8String]);
    Base::setResourceManager(_resourceManager.get());
}

template <typename Base>
inline std::string Apple<Base>::userStoragePath() const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    return std::string{[applicationSupportDirectory UTF8String]} + '/' + this->organization() + '/' + this->name();
}

template <typename Base>
inline std::string Apple<Base>::deviceModel() const {
    utsname systemInfo;
    uname(&systemInfo);
    return systemInfo.machine;
}

}}}

#endif
