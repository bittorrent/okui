#pragma once

#include "onair/okui/config.h"

#include "onair/okui/opengl/OpenGL.h"

#include "scraps/Cache.h"

namespace onair {
namespace okui {
namespace opengl {

struct TextureCacheEntry {
    TextureCacheEntry(GLuint id_) : id{id_} {}

    TextureCacheEntry(TextureCacheEntry&& other) { std::swap(id, other.id); }

    ~TextureCacheEntry() {
        if (id) {
            glDeleteTextures(1, &id);
        }
    }

    GLuint id = 0;
};

using TextureCache = scraps::Cache<TextureCacheEntry>;

} // namespace opengl
} // namespace okui
} // namespace onair
