#include "onair/okui/platforms/Apple.h"

#if __APPLE__

#import <Foundation/Foundation.h>

namespace onair {
namespace okui {
namespace platforms {

std::string Apple::userStoragePath(const char* application, const char* organization) const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    return std::string([applicationSupportDirectory UTF8String]) + '/' + organization + '/' + application;
}

}}}

#endif
