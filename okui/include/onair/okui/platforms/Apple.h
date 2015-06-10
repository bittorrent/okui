#pragma once

#include "onair/okui/config.h"

#if __APPLE__

#include "onair/okui/Platform.h"

namespace onair {
namespace okui {
namespace platforms {

/**
* Provides some native Platform overrides for iOS and OS X.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class Apple : public Platform {
public:
    virtual std::string userStoragePath(const char* application, const char* organization) const override;

    virtual KeyModifier defaultShortcutModifier() const override { return KeyModifier::kSuper; }

    virtual std::string resourcePath() const override;
};

}}}

#endif
