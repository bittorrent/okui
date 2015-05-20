#pragma once

#include "bittorrent/ui/config.h"
#include "bittorrent/ui/Platform.h"

#if BT_MAC_OS_X
#include "bittorrent/ui/platforms/OSX.h"
#elif BT_IOS
#include "bittorrent/ui/platforms/IOS.h"
#endif

namespace bittorrent {
namespace ui {
namespace platforms {

#if BT_MAC_OS_X
using Native = OSX;
#elif BT_IOS
using Native = IOS;
#else
using Native = Platform;
#endif

}}}
