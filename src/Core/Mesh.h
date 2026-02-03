#pragma once

#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

enum class PrimitiveType {
    TRIANGLES = GL_TRIANGLES,
    LINES = GL_LINES,
    LINE_STRIP = GL_LINE_STRIP,
    POINTS = GL_POINTS
};

struct LayoutAttribute {
    GLenum type;
    int count;
    bool normalized;

    LayoutAttribute(const GLenum type, const int count, const bool normalized = false) :
        type(type), count(count), normalized(normalized) {}

    size_t GetSize() const {
        size_t typeSize = 0;
        // clang-format off
        switch (type) {
        case GL_FLOAT:          typeSize = sizeof(float);          break;
        case GL_INT:            typeSize = sizeof(int);            break;
        case GL_UNSIGNED_INT:   typeSize = sizeof(unsigned int);   break;
        case GL_BYTE:           typeSize = sizeof(char);           break;
        case GL_UNSIGNED_BYTE:  typeSize = sizeof(unsigned char);  break;
        case GL_SHORT:          typeSize = sizeof(short);          break;
        case GL_UNSIGNED_SHORT: typeSize = sizeof(unsigned short); break;
        default:                typeSize = sizeof(float);          break;
        }
        // clang-format on
        return typeSize * count;
    }
};

struct VertexLayout {
    std::vector<LayoutAttribute> attributes;

    VertexLayout() = default;
    VertexLayout(const std::initializer_list<LayoutAttribute> attrs) : attributes(attrs) {}

    size_t GetStride() const {
        size_t stride = 0;
        for (const auto& attr : attributes) {
            stride += attr.GetSize();
        }
        return stride;
    }

    // clang-format off
    static VertexLayout Position3D()             { return {{GL_FLOAT, 3}}; }
    static VertexLayout PositionNormal()         { return {{GL_FLOAT, 3}, {GL_FLOAT, 3}}; }
    static VertexLayout PositionTexCoord()       { return {{GL_FLOAT, 3}, {GL_FLOAT, 2}}; }
    static VertexLayout PositionColor()          { return {{GL_FLOAT, 3}, {GL_FLOAT, 4}}; }
    static VertexLayout PositionNormalTexCoord() { return {{GL_FLOAT, 3}, {GL_FLOAT, 3}, {GL_FLOAT, 2}};
        // clang-format on
    }
};

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    Mesh& SetVertices(const std::vector<float>& data);
    Mesh& SetVertices(std::vector<float>&& data);

    Mesh& SetIndices(const std::vector<uint32_t>& data);
    Mesh& SetIndices(std::vector<uint32_t>&& data);

    Mesh& SetLayout(const VertexLayout& layout_);
    Mesh& SetPrimitiveType(PrimitiveType type);

    Mesh& Upload();

    void Draw() const;

    static Mesh PlanarGrid(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max);

private:
    void SetupVertexAttributes() const;
    void Cleanup();
    size_t GetVertexCount() const;

private:
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    VertexLayout layout = VertexLayout::Position3D();
    PrimitiveType primitiveType = PrimitiveType::TRIANGLES;

    GLuint vao = GL_NONE;
    GLuint vbo = GL_NONE;
    GLuint ibo = GL_NONE;
};
