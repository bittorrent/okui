#pragma once

#include "onair/okui/config.h"

namespace onair {
namespace okui {

/**
* This is the type used to represent command ids.
*/
using Command = uint64_t;
using CommandContext = int;

namespace {

constexpr Command kCommandNone = 0;

/**
* Commands in the onair::okui namespace are OR'd with this value to avoid
* collisions with user or third-party commands.
*/
constexpr Command kCommandPrefixMask = UINT64_C(0x33359c3d) << 32;

constexpr Command kCommandCopy  = kCommandPrefixMask | 1;
constexpr Command kCommandPaste = kCommandPrefixMask | 2;
constexpr Command kCommandQuit  = kCommandPrefixMask | 3;

} // namespace

}} // namespace onair::okui
