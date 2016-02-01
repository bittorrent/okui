#pragma once

#include "onair/okui/config.h"

#if __APPLE__

#include "onair/okui/applications/SDL.h"

namespace onair {
namespace okui {
namespace applications {

/**
* Provides some native Application overrides for iOS and OS X.
*
* This isn't intended to be a full implementation, but can be used to add native support to other implementations.
*/
class Apple : public SDL {
public:
    virtual ~Apple() {}

    virtual std::string userStoragePath() const override;

    virtual KeyModifier defaultShortcutModifier() const override { return KeyModifier::kSuper; }

    virtual std::string deviceModel() const override;

protected:    
    virtual std::unique_ptr<ResourceManager> defaultResourceManager() const override;
};

}}}

#endif
