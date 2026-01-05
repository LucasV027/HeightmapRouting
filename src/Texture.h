#pragma once

#include "Mat.h"
#include "Window.h"

class Texture {
public:
    // TODO: More formats
    enum class Format {
        Float1,
        Float3,
    };

    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void Bind(uint32_t unit = 0) const;

    uint32_t Width() const { return width; }
    uint32_t Height() const { return height; }
    GLuint Handle() const { return handle; }

    template <typename T>
    static Texture From(const Mat<T>& mat) {
        Format format;

        // clang-format off
        if constexpr      (std::is_same_v<T, float>)     format = Format::Float1;
        else if constexpr (std::is_same_v<T, glm::vec3>) format = Format::Float3;
        else static_assert(false, "Unsupported mat texture conversion");
        // clang-format on

        return {mat.Width(), mat.Height(), format, mat.Data()};
    }

private:
    Texture(uint32_t width, uint32_t height, Format format, const void* data);

    void Cleanup();

    static GLenum GetInternalFormat(Format format);
    static GLenum GetDataFormat(Format format);
    static GLenum GetDataType(Format format);

private:
    GLuint handle = GL_NONE;
    uint32_t width = 0;
    uint32_t height = 0;
    Format format = Format::Float1;
};
