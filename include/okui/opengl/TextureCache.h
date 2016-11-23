#pragma once

#include <okui/config.h>

#include <okui/opengl/opengl.h>

#include <scraps/Cache.h>

namespace okui::opengl {

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

} // namespace okui::opengl
