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

#include <scraps/utility.h>

#include <stdts/any.h>

namespace okui {

/**
* This is the type used to represent command ids.
*/
using Command = uint64_t;
using CommandContext = stdts::any;

inline namespace literals {
    constexpr Command operator "" _cmd(const char* str, size_t length) {
        return scraps::FNV1A64(str, length);
    }
}

constexpr Command kCommandNone = 0;

constexpr Command kCommandCopy                   = "copy"_cmd;
constexpr Command kCommandCut                    = "cut"_cmd;
constexpr Command kCommandPaste                  = "paste"_cmd;

constexpr Command kCommandQuit                   = "quit"_cmd;
constexpr Command kCommandSelectAll              = "select all"_cmd;
constexpr Command kCommandBack                   = "back"_cmd;

constexpr Command kCommandToggleFullscreenWindow = "toggle fullscreen window"_cmd;
constexpr Command kCommandToggleMinimizeWindow   = "toggle window minimize"_cmd;
constexpr Command kCommandToggleMaximizeWindow   = "toggle window maximize"_cmd;
constexpr Command kCommandBringAllWindowsToFront = "bring all windows to front"_cmd;

} // namespace okui
