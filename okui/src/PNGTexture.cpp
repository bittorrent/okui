#include "onair/okui/PNGTexture.h"

#include <png.h>

namespace onair {
namespace okui {

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

void PNGTexture::setData(std::shared_ptr<const std::string> data) {
    _data = data;
    _cacheEntry = nullptr;

    PNGInput input(_data->data(), _data->size());

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        ONAIR_LOGF_ERROR("error reading png data");
        png_destroy_read_struct(&png, &info, nullptr);
        return;
    }

    png_set_read_fn(png, &input, &PNGRead);

    png_read_info(png, info);

    _width = png_get_image_width(png, info);
    _height = png_get_image_height(png, info);

    png_destroy_read_struct(&png, &info, nullptr);
}

GLuint PNGTexture::load(opengl::TextureCache* textureCache) {
    PNGInput input(_data->data(), _data->size());

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png))) {
        ONAIR_LOGF_ERROR("error reading png data");
        png_destroy_read_struct(&png, &info, nullptr);
        return 0;
    }

    png_set_read_fn(png, &input, &PNGRead);

    png_read_info(png, info);

    auto colorType = png_get_color_type(png, info);
    auto glFormat =
#if GL_RED && GL_RG
        colorType == PNG_COLOR_TYPE_GRAY ? GL_RED :
        colorType == PNG_COLOR_TYPE_GRAY_ALPHA ? GL_RG :
#else
        colorType == PNG_COLOR_TYPE_GRAY ? GL_LUMINANCE :
        colorType == PNG_COLOR_TYPE_GRAY_ALPHA ? GL_LUMINANCE_ALPHA :
#endif
        colorType == PNG_COLOR_TYPE_RGB ? GL_RGB :
        colorType == PNG_COLOR_TYPE_RGB_ALPHA ? GL_RGBA :
    0;

    if (!glFormat) {
        ONAIR_LOGF_ERROR("unsupported color type (%d)", static_cast<int>(colorType));
        png_destroy_read_struct(&png, &info, nullptr);
        return 0;
    }

    int bytesPerRow = png_get_rowbytes(png, info);

    int bitDepth = png_get_bit_depth(png, info);
    if (bitDepth != 8 && bitDepth != 16) {
        ONAIR_LOGF_ERROR("unsupported bit depth");
        png_destroy_read_struct(&png, &info, nullptr);
        return 0;
    }

    const auto components = ((colorType & PNG_COLOR_MASK_COLOR) ? 3 : 1) + ((colorType & PNG_COLOR_MASK_ALPHA) ? 1 : 0);
    if (bytesPerRow != components * (bitDepth >> 3) * _width) {
        assert(false);
        ONAIR_LOGF_ERROR("unknown error");
        png_destroy_read_struct(&png, &info, nullptr);
        return 0;
    }

#if OPENGL_ES
    if (bitDepth == 16) {
        // opengl es doesn't do 16-bit textures
        bitDepth = 8;
        png_set_strip_16(png);
        bytesPerRow = components * (bitDepth >> 3) * _width;
    }
#endif

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

    // align rows to 4-byte boundaries
    if (bytesPerRow % 4) {
        bytesPerRow += 4 - (bytesPerRow % 4);
    }

    std::vector<png_byte> image;
    image.resize(bytesPerRow * _height);

    std::vector<png_byte*> rowPointers;
    for (int i = 0; i < _height; ++i) {
        rowPointers.emplace_back(image.data() + i * bytesPerRow);
    }

    png_read_image(png, rowPointers.data());
    png_destroy_read_struct(&png, &info, nullptr);

    // image now contains our raw bytes. send it to the gpu

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, _width, _height, 0, glFormat, bitDepth == 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, image.data());

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if GL_RED && GL_RG
    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

        if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
        }
    }
#endif

    ONAIR_OKUI_GL_ERROR_CHECK();

    _cacheEntry = textureCache->add(texture, texture);

    return _cacheEntry->id;
}

}}
