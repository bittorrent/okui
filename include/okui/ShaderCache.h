#pragma once

#include "okui/config.h"

#include "scraps/Cache.h"

#include "okui/Shader.h"

namespace okui {

using ShaderCacheEntry = std::unique_ptr<Shader>;
using ShaderCache = scraps::Cache<ShaderCacheEntry>;

} // namespace okui
