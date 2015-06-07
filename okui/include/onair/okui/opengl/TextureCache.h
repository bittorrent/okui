#pragma once

#include "onair/okui/config.h"

#include "onair/okui/Cache.h"
#include "onair/okui/opengl/OpenGL.h"

namespace onair {
namespace okui {
namespace opengl {

struct TextureCacheEntry {
	TextureCacheEntry(GLuint id) : id(id) {}
	
	TextureCacheEntry(TextureCacheEntry&& other) {
		std::swap(id, other.id);
	}
	
	~TextureCacheEntry() {
		if (id) {
			glDeleteTextures(1, &id);
		}
	}
	
	GLuint id = 0;
};

using TextureCache = Cache<TextureCacheEntry>;

}}}
