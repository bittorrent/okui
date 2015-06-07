#pragma once

#include "onair/okui/config.h"

#if ONAIR_IOS

#include "onair/okui/platforms/Apple.h"

namespace onair {
namespace okui {
namespace platforms {

/**
* Provides some native Platform overrides for iOS.
*
* This isn't intended to be a full implementation, but can be used as a base for other implementations.
*/
class IOS : public Apple {
public:
    virtual bool canOpenURL(const char* url) const override;
    virtual bool openURL(const char* url) override;
    
    virtual void openDialog(Window* window, const char* title, const char* message, const std::vector<std::string>& buttons, std::function<void(int)> action = std::function<void(int)>()) override;
};

}}}

#endif
