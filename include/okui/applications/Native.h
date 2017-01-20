/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <okui/config.h>
#include <okui/Application.h>

#if SCRAPS_MACOS
#include <okui/applications/OSX.h>
#elif SCRAPS_IOS
#include <okui/applications/IOS.h>
#elif SCRAPS_TVOS
#include <okui/applications/TvOS.h>
#elif SCRAPS_ANDROID
#include <okui/applications/Android.h>
#else
#include <okui/Application.h>
#endif

namespace okui::applications {

#define ONAIR_OKUI_HAS_NATIVE_APPLICATION 1

template <typename Base>
#if SCRAPS_MACOS
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

} // namespace okui::applications
