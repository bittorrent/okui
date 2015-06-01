#pragma once

#include "bittorrent/ui/config.h"

namespace bittorrent {
namespace ui {

/**
* This is the type used to represent command ids.
*/
using Command = uint64_t;
using CommandContext = int;

static constexpr Command kCommandNone = 0;

/**
* Commands in the bittorrent::ui namespace are OR'd with this value to avoid
* collisions with user or third-party commands.
*/
static constexpr Command kCommandPrefixMask = UINT64_C(0x33359c3d) << 32;

static constexpr Command kCommandCopy  = kCommandPrefixMask | 1;
static constexpr Command kCommandPaste = kCommandPrefixMask | 2;
static constexpr Command kCommandQuit  = kCommandPrefixMask | 3;

}}
