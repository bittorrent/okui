#pragma once

#include <okui/config.h>

#include <okui/Shader.h>

#include <scraps/Cache.h>

namespace okui {

using ShaderCacheEntry = std::unique_ptr<Shader>;
using ShaderCache = scraps::Cache<ShaderCacheEntry>;

} // namespace okui
