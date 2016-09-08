#include "okui/FileTexture.h"

#include <gsl.h>
#include <png.h>
#include <turbojpeg.h>

namespace okui {

namespace {

#if OPENGL_ES
    constexpr bool IsPowerOfTwo(int x) {
        return (x != 0) && !(x & (x - 1));
    }

    constexpr int NextPowerOfTwo(int x) {
        if (x < 0) {
            return 0;
        }
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x + 1;
    }
#endif // OPENGL_ES

    struct PNGInput {
        PNGInput(const void* data, size_t length) : data(data), length(length) {}

        const void* data;
        size_t length;
        size_t position = 0;
    };

    void PNGRead(png_structp png, png_bytep data, png_size_t length) {
        PNGInput* input = reinterpret_cast<PNGInput*>(png_get_io_ptr(png));
        if (length > input->length - input->position) {
            png_error(png, "read error (not enough data)");
        } else {
            memcpy(data, reinterpret_cast<const char*>(input->data) + input->position, length);
            input->position += length;
        }
    }

    void PNGError(png_structp png, png_const_charp message) {
        longjmp(png_jmpbuf(png), 1);
    }

    void PNGWarning(png_structp png, png_const_charp message) { /* nop */ }

} // anonymous namespace

FileTexture::~FileTexture() {
    if (_id) {
        glDeleteTextures(1, &_id);
    }
}

void FileTexture::setData(std::shared_ptr<const std::string> data, std::string name) {
    _name = std::move(name);
    _data = std::move(data);

    if (_readPNGMetadata()) {
        _type = Type::kPNG;
    } else if (_readJPEGMetadata()) {
        _type = Type::kJPEG;
    } else {
        SCRAPS_LOG_ERROR("unknown file type for {}", name);
        _type = Type::kUnknown;
        _data = nullptr;
    }
}

bool FileTexture::decompress() {
    switch (_type) {
        case Type::kPNG:  return _loadPNG();
        case Type::kJPEG: return _loadJPEG();
        default:          return false;
    }
}

void FileTexture::load() {
    if (_decompressedData.empty()) { return; }

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexImage2D(GL_TEXTURE_2D, 0, _textureType.format, _allocatedWidth, _allocatedHeight, 0, _textureType.format, _textureType.type, _decompressedData.data());

#if OPENGL_ES
    bool useMipmaps = IsPowerOfTwo(_allocatedWidth) && IsPowerOfTwo(_allocatedHeight);
#else
    constexpr bool useMipmaps = true;
#endif

    if (useMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if GL_RED && GL_RG
    if (_textureType.format == GL_RED || _textureType.format == GL_RG) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

        if (_textureType.format == GL_RG) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
        }
    }
#endif

    SCRAPS_GL_ERROR_CHECK();

    _decompressedData.clear();
}

bool FileTexture::_readPNGMetadata() {
    PNGInput input(_data->data(), _data->size());

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGError, PNGWarning);
    png_infop info = png_create_info_struct(png);
    auto _ = gsl::finally([&]{ png_destroy_read_struct(&png, &info, nullptr); });

    if (setjmp(png_jmpbuf(png))) {
        return false;
    }

    png_set_read_fn(png, &input, &PNGRead);

    png_read_info(png, info);

    _width = png_get_image_width(png, info);
    _height = png_get_image_height(png, info);

    return true;
}

bool FileTexture::_readJPEGMetadata() {
    auto decompressor = tjInitDecompress();
    auto _ = gsl::finally([&]{ tjDestroy(decompressor); });

    int width{0}, height{0}, jpegSubsamp{0}, jpegColorspace{0};
    if (!tjDecompressHeader3(decompressor, reinterpret_cast<const unsigned char*>(_data->data()), _data->size(), &width, &height, &jpegSubsamp, &jpegColorspace)) {
        _width = width;
        _height = height;
        return true;
    }

    return false;
}

bool FileTexture::_loadPNG() {
    PNGInput input(_data->data(), _data->size());

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PNGError, PNGWarning);
    png_infop info = png_create_info_struct(png);
    auto _ = gsl::finally([&]{ png_destroy_read_struct(&png, &info, nullptr); });

    if (setjmp(png_jmpbuf(png))) {
        SCRAPS_LOG_ERROR("png decompression error for {}: error reading png data", _name);
        return false;
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
        SCRAPS_LOG_ERROR("png decompression error for {}: unsupported color type ({})", _name, static_cast<int>(colorType));
        return false;
    }

    int bytesPerRow = png_get_rowbytes(png, info);

    int bitDepth = png_get_bit_depth(png, info);
    if (bitDepth != 8 && bitDepth != 16) {
        SCRAPS_LOG_ERROR("png decompression error for {}: unsupported bit depth", _name);
        return false;
    }

    const auto components = ((colorType & PNG_COLOR_MASK_COLOR) ? 3 : 1) + ((colorType & PNG_COLOR_MASK_ALPHA) ? 1 : 0);
    if (bytesPerRow != components * (bitDepth >> 3) * _width) {
        assert(false);
        SCRAPS_LOG_ERROR("png decompression error for {}: unknown error", _name);
        return false;
    }

#if OPENGL_ES
    if (bitDepth == 16) {
        // opengl es doesn't do 16-bit textures
        bitDepth = 8;
        png_set_strip_16(png);
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

#if OPENGL_ES
    // make powers of 2 so we can use mipmaps
    _allocatedWidth = NextPowerOfTwo(_width);
    _allocatedHeight = NextPowerOfTwo(_height);
#else
    _allocatedWidth = _width;
    _allocatedHeight = _height;
#endif

    bytesPerRow = components * (bitDepth >> 3) * _allocatedWidth;

    // align rows to 4-byte boundaries
    if (bytesPerRow % 4) {
        bytesPerRow += 4 - (bytesPerRow % 4);
    }

    _decompressedData.resize(bytesPerRow * _allocatedHeight);

    std::vector<png_byte*> rowPointers;
    for (int i = 0; i < _allocatedHeight; ++i) {
        rowPointers.emplace_back(_decompressedData.data() + i * bytesPerRow);
    }

    png_read_image(png, rowPointers.data());

    static_assert(sizeof(png_byte) == sizeof(uint8_t), "sizeof(png_byte) must be sizeof(char)");

    _textureType.type = bitDepth == 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT;
    _textureType.format = glFormat;

    return true;
}

bool FileTexture::_loadJPEG() {
    auto decompressor = tjInitDecompress();
    auto _ = gsl::finally([&]{ tjDestroy(decompressor); });

    int width{0}, height{0}, jpegSubsamp{0}, jpegColorspace{0};
    if (tjDecompressHeader3(decompressor, reinterpret_cast<const unsigned char*>(_data->data()), _data->size(), &width, &height, &jpegSubsamp, &jpegColorspace)) {
        SCRAPS_LOG_ERROR("error reading jpeg header {}: {}", _name, tjGetErrorStr());
        return false;
    }

#if OPENGL_ES
    // make powers of 2 so we can use mipmaps
    _allocatedWidth = NextPowerOfTwo(width);
    _allocatedHeight = NextPowerOfTwo(height);
#else
    _allocatedWidth = _width;
    _allocatedHeight = _height;
#endif

    auto pixelFormat = jpegColorspace == TJCS_GRAY ? TJPF_GRAY : TJPF_RGB;
    auto bytesPerRow = TJPAD(_allocatedWidth * tjPixelSize[pixelFormat]);

    _decompressedData.resize(bytesPerRow * _allocatedHeight);

    if (tjDecompress2(decompressor, reinterpret_cast<const unsigned char*>(_data->data()), _data->size(), _decompressedData.data(), width, bytesPerRow, height, pixelFormat, 0)) {
        SCRAPS_LOG_ERROR("jpeg decompression error {}: {}", _name, tjGetErrorStr());
        // If there's an error loading the header it's unrecoverable, but an error here, decompressing
        // the body, should just show whatever data was successfully decompressed.
    }

    _textureType.type = GL_UNSIGNED_BYTE;

#if GL_RED && GL_RG
    _textureType.format = jpegColorspace == TJCS_GRAY ? GL_RED : GL_RGB;
#else
    _textureType.format = jpegColorspace == TJCS_GRAY ? GL_LUMINANCE : GL_RGB;
#endif

    return true;
}

} // namespace okui
