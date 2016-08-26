#pragma once

#include "okui/config.h"

#if SCRAPS_TVOS

#include "okui/applications/IOS.h"

namespace okui {
namespace applications {

/**
* Provides some native Platform overrides for TvOS.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
template <typename Base>
class TvOS : public IOS<Base> {
public:
    virtual std::string userStoragePath() const override {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        return std::string{[[paths firstObject] UTF8String]};
    }

    virtual void keyUp(KeyCode key, KeyModifiers mod, bool repeat) override {
        if (key == KeyCode::kMenu) {
            return this->command(kCommandBack);
        }
        IOS<Base>::keyUp(key, mod, repeat);
    }
};


} } // namespace okui::applications

#endif
