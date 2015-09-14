#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Application.h"

#if ONAIR_MAC_OS_X
#include "onair/okui/applications/OSX.h"
#elif ONAIR_IOS
#include "onair/okui/applications/IOS.h"
#elif ONAIR_TVOS
#include "onair/okui/applications/IOS.h"
#elif ONAIR_ANDROID
#include "onair/okui/applications/Android.h"
#endif

namespace onair {
namespace okui {
namespace applications {

#if ONAIR_MAC_OS_X
using Native = OSX;
#elif ONAIR_IOS
using Native = IOS;
#elif ONAIR_ANDROID
using Native = Android;
#else
using Native = Application;
#endif

} // namespace applications
} // namespace okui
} // namespace onair
