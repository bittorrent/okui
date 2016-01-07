#include "onair/okui/applications/TvOS.h"

#if ONAIR_TVOS

#import <Foundation/Foundation.h>

namespace onair {
namespace okui {
namespace applications {

std::string TvOS::userStoragePath() const {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    return std::string{[[paths firstObject] UTF8String]};
}

} // namespace applications
} // namespace okui
} // namespace onair

#endif
