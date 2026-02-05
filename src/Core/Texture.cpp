#include "Texture.h"

#include <utility>

Texture::Texture(const uint32_t width,
                 const uint32_t height,
                 const Format format,
                 const void* data) : width(width), height(height), format(format) {

    glCreateTextures(GL_TEXTURE_2D, 1, &handle);

    const auto internalFormat = GetInternalFormat(format);
    glTextureStorage2D(handle, 1, internalFormat, width, height);

    if (data) {
        const auto dataFormat = GetDataFormat(format);
        const auto dataType = GetDataType(format);
        glTextureSubImage2D(handle, 0, 0, 0, width, height, dataFormat, dataType, data);
    }

    const auto filter = GetFiltering(format);
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, filter);

    glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

Texture::~Texture() {
    Cleanup();
}

Texture::Texture(Texture&& other) noexcept :
    handle(other.handle), width(other.width), height(other.height), format(other.format) {
    other.handle = GL_NONE;
    other.width = 0;
    other.height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        Cleanup();

        handle = other.handle;
        width = other.width;
        height = other.height;
        format = other.format;

        other.handle = GL_NONE;
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

void Texture::Bind(const uint32_t unit) const {
    glBindTextureUnit(unit, handle);
}


void Texture::Cleanup() {
    if (handle != GL_NONE) {
        glDeleteTextures(1, &handle);
        handle = GL_NONE;
    }
}

GLenum Texture::GetInternalFormat(const Format format) {
    // clang-format off
    switch (format) {
    case Format::U8_1:  return GL_R8UI;
    case Format::F32_1: return GL_R32F;
    case Format::F32_3: return GL_RGB32F;
    }
    std::unreachable();
    // clang-format on
}

GLenum Texture::GetDataFormat(const Format format) {
    // clang-format off
    switch (format) {
    case Format::U8_1:  return GL_RED_INTEGER;
    case Format::F32_1: return GL_RED;
    case Format::F32_3: return GL_RGB;
    }
    std::unreachable();
    // clang-format on
}

GLenum Texture::GetDataType(const Format format) {
    // clang-format off
    switch (format) {
    case Format::U8_1: return GL_UNSIGNED_BYTE;
    case Format::F32_1:
    case Format::F32_3: return GL_FLOAT;
    }
    std::unreachable();
    // clang-format on
}

GLenum Texture::GetFiltering(const Format format) {
    // clang-format off
    switch (format) {
    case Format::U8_1:  return GL_NEAREST;
    case Format::F32_1:
    case Format::F32_3: return GL_LINEAR;
    }
    std::unreachable();
    // clang-format on
}
