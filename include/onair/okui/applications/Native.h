#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Application.h"

#if SCRAPS_MAC_OS_X
#include "onair/okui/applications/OSX.h"
#elif SCRAPS_IOS
#include "onair/okui/applications/IOS.h"
#elif SCRAPS_TVOS
#include "onair/okui/applications/TvOS.h"
#elif SCRAPS_ANDROID
#include "onair/okui/applications/Android.h"
#else
#include "onair/okui/Application.h"
#endif

namespace onair {
namespace okui {
namespace applications {

#define ONAIR_OKUI_HAS_NATIVE_APPLICATION 1

template <typename Base>
#if SCRAPS_OS_X
using Native = OSX<Base>;
#elif SCRAPS_IOS
using Native = IOS<Base>;
#elif SCRAPS_TVOS
using Native = TvOS<Base>;
#elif SCRAPS_ANDROID
using Native = Android<Base>;
#else
#undef ONAIR_OKUI_HAS_NATIVE_APPLICATION
using Native = Base;
#endif

} // namespace applications
} // namespace okui
} // namespace onair
