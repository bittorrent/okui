#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Application.h"

#if ONAIR_MAC_OS_X
#include "onair/okui/applications/OSX.h"
#elif ONAIR_IOS
#include "onair/okui/applications/IOS.h"
#elif ONAIR_TVOS
#include "onair/okui/applications/TvOS.h"
#elif ONAIR_ANDROID
#include "onair/okui/applications/Android.h"
#else
#include "onair/okui/Application.h"
#endif

namespace onair {
namespace okui {
namespace applications {

#define ONAIR_OKUI_HAS_NATIVE_APPLICATION 1

template <typename Base>
#if ONAIR_OS_X
using Native = OSX<Base>;
#elif ONAIR_IOS
using Native = IOS<Base>;
#elif ONAIR_TVOS
using Native = TvOS<Base>;
#elif ONAIR_ANDROID
using Native = Android<Base>;
#else
#undef ONAIR_OKUI_HAS_NATIVE_APPLICATION
using Native = Base;
#endif

} // namespace applications
} // namespace okui
} // namespace onair
