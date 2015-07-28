#include "onair/okui/applications/Apple.h"

#if __APPLE__

#import <Foundation/Foundation.h>

namespace onair {
namespace okui {
namespace applications {

std::string Apple::userStoragePath() const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* applicationSupportDirectory = [paths firstObject];
    return std::string{[applicationSupportDirectory UTF8String]} + '/' + organization() + '/' + name();
}

std::string Apple::resourcePath() const {
	NSString* path = [[NSBundle mainBundle] resourcePath];
	return std::string{[path UTF8String]};
}

}}}

#endif
