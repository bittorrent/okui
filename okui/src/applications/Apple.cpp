#include "onair/okui/applications/Apple.h"

#include "onair/okui/FileResourceManager.h"

#if __APPLE__

#import <Foundation/Foundation.h>

#include <sys/utsname.h>

namespace onair {
namespace okui {
namespace applications {

std::unique_ptr<ResourceManager> Apple::defaultResourceManager() const {
    	NSString* path = [[NSBundle mainBundle] resourcePath];
    return std::make_unique<FileResourceManager>([path UTF8String]);
}

std::string Apple::userStoragePath() const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    return std::string{[applicationSupportDirectory UTF8String]} + '/' + organization() + '/' + name();
}

std::string Apple::deviceModel() const {
    utsname systemInfo;
    uname(&systemInfo);
    return systemInfo.machine;
}

}}}

#endif
