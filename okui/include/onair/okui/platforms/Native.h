#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Platform.h"

#if ONAIR_MAC_OS_X
#include "onair/okui/platforms/OSX.h"
#elif ONAIR_IOS
#include "onair/okui/platforms/IOS.h"
#endif

namespace onair {
namespace okui {
namespace platforms {

#if ONAIR_MAC_OS_X
using Native = OSX;
#elif ONAIR_IOS
using Native = IOS;
#else
using Native = Platform;
#endif

}}}
