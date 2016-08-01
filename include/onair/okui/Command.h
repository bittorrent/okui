#pragma once

#include "onair/okui/config.h"

#include "scraps/utility.h"
#include "scraps/stdts/any.h"

namespace onair {
namespace okui {

/**
* This is the type used to represent command ids.
*/
using Command = uint64_t;
using CommandContext = scraps::stdts::any;

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

} } //namespace onair::okui
