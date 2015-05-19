#include "bittorrent/ui/Texture.h"

#include <png.h>

namespace bittorrent {
namespace ui {

struct PNGInput {
    PNGInput(const void* data, size_t length) : data(data), length(length) {}

    const void* data;
    size_t length;
    size_t position = 0;
};

static void PNGRead(png_structp png, png_bytep data, png_size_t length) {
    PNGInput* input = reinterpret_cast<PNGInput*>(png_get_io_ptr(png));
    if (length > input->length - input->position) {
        png_error(png, "read error (not enough data)");
    } else {
        memcpy(data, reinterpret_cast<const char*>(input->data) + input->position, length);
        input->position += length;
    }
}

Texture::Texture(std::shared_ptr<const std::string> data) : _data(data) {
    PNGInput input(_data->data(), _data->size());

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        BT_LOG_ERROR("error reading png data");
        png_destroy_read_struct(&png, &info, nullptr);
        return;
    }

    png_set_read_fn(png, &input, &PNGRead);

    png_read_info(png, info);

    _width = png_get_image_width(png, info);
    _height = png_get_image_height(png, info);

    png_destroy_read_struct(&png, &info, nullptr);
}

int Texture::width() {
    return _width;
}

int Texture::height() {
    return _height;
}

GLuint Texture::load(opengl::TextureCache* textureCache) {
    PNGInput input(_data->data(), _data->size());

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        BT_LOG_ERROR("error reading png data");
        png_destroy_read_struct(&png, &info, nullptr);
        return 0;
    }

    png_set_read_fn(png, &input, &PNGRead);

    png_read_info(png, info);

    auto colorType = png_get_color_type(png, info);
    int bytesPerRow = png_get_rowbytes(png, info);
    
    int bitDepth = png_get_bit_depth(png, info);
    if (bitDepth != 8 && bitDepth != 16) {
        BT_LOG_ERROR("unsupported bit depth");
        png_destroy_read_struct(&png, &info, nullptr);
        return 0;
    }
        
    if (bitDepth > 8) {
        // libpng stores pixels as big endian. we need them in native endianness for opengl
        union {
            uint32_t i;
            char c[4];
        } u = {0x01020304};

        if (u.c[0] != 1) {
            // we're not big endian. swap
            png_set_swap(png);
        }
    }

    BT_ASSERT(bytesPerRow == ((colorType & PNG_COLOR_MASK_ALPHA) ? 4 : 3) * (bitDepth >> 3) * _width);

    // align rows to 4-byte boundaries
    if (bytesPerRow % 4) {
        bytesPerRow += 4 - (bytesPerRow % 4);
    }

    png_byte* image = reinterpret_cast<png_byte*>(malloc(bytesPerRow * _height));
    png_bytep* rowPointers = reinterpret_cast<png_bytep*>(malloc(_height * sizeof(png_bytep)));

    for (int i = 0; i < _height; ++i) {
        rowPointers[i] = image + i * bytesPerRow;
    }

    png_read_image(png, rowPointers);

    free(rowPointers);

    png_destroy_read_struct(&png, &info, nullptr);

    // image now contains our raw bytes. send it to the gpu

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    auto format = (colorType & PNG_COLOR_MASK_ALPHA) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, bitDepth == 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, image);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _cacheEntry = textureCache->add(texture, texture);

    free(image);

    return _cacheEntry->id;
}

}}
