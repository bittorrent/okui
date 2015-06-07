#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Cache.h"
#include "onair/okui/Shader.h"

namespace onair {
namespace okui {

using ShaderCacheEntry = std::unique_ptr<Shader>;
using ShaderCache = Cache<ShaderCacheEntry>;

}}
