#include "onair/okui/applications/Apple.h"

#include "onair/okui/FileResourceManager.h"

#if __APPLE__

#import <Foundation/Foundation.h>

namespace onair {
namespace okui {
namespace applications {

void Apple::initialize() {
    if (!resourceManager()) {
        	NSString* path = [[NSBundle mainBundle] resourcePath];
        _resourceManager.reset(new FileResourceManager([path UTF8String]));
        ONAIR_LOGF_DEBUG("using resources in %s", [path UTF8String]);
        setResourceManager(_resourceManager.get());
    }

    Application::initialize();
}

std::string Apple::userStoragePath() const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    return std::string{[applicationSupportDirectory UTF8String]} + '/' + organization() + '/' + name();
}

}}}

#endif
