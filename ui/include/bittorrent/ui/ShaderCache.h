#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Cache.h"
#include "bittorrent/ui/Shader.h"

namespace bittorrent {
namespace ui {

using ShaderCacheEntry = std::unique_ptr<Shader>;
using ShaderCache = Cache<ShaderCacheEntry>;

}}