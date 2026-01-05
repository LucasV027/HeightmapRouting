#include "Texture.h"


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

    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    switch (format) {
    case Format::Float1:
        return GL_R32F;
    case Format::Float3:
        return GL_RGB32F;
    }
    return GL_R32F;
}

GLenum Texture::GetDataFormat(const Format format) {
    switch (format) {
    case Format::Float1:
        return GL_RED;
    case Format::Float3:
        return GL_RGB;
    }
    return GL_RED;
}

GLenum Texture::GetDataType(const Format format) {
    switch (format) {
    case Format::Float1:
    case Format::Float3:
        return GL_FLOAT;
    }
    return GL_FLOAT;
}
