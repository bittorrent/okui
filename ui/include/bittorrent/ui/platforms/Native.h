#pragma once

#include "bittorrent/ui/config.h"
#include "bittorrent/ui/Platform.h"

#if BT_MAC_OS_X
#include "bittorrent/ui/platforms/MacOSX.h"
#endif

namespace bittorrent {
namespace ui {
namespace platforms {

#if BT_MAC_OS_X
using Native = MacOSX;
#else
using Native = Platform;
#endif

}}}
