#pragma once

#include "bittorrent/ui/config.h"

#include "bittorrent/ui/Cache.h"
#include "bittorrent/ui/opengl/OpenGL.h"

namespace bittorrent {
namespace ui {
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