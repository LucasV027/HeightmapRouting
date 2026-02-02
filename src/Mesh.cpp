#include "Mesh.h"

Mesh::~Mesh() {
    Cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept :
    vertices(std::move(other.vertices)), indices(std::move(other.indices)),
    primitiveType(other.primitiveType), vao(other.vao), vbo(other.vbo), ibo(other.ibo) {
    other.vao = GL_NONE;
    other.vbo = GL_NONE;
    other.ibo = GL_NONE;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Cleanup();

        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        primitiveType = other.primitiveType;
        vao = other.vao;
        vbo = other.vbo;
        ibo = other.ibo;

        other.vao = GL_NONE;
        other.vbo = GL_NONE;
        other.ibo = GL_NONE;
    }
    return *this;
}

Mesh& Mesh::SetVertices(const std::vector<float>& data) {
    vertices = data;
    return *this;
}

Mesh& Mesh::SetVertices(std::vector<float>&& data) {
    vertices = std::move(data);
    return *this;
}

Mesh& Mesh::SetIndices(const std::vector<uint32_t>& data) {
    indices = data;
    return *this;
}

Mesh& Mesh::SetIndices(std::vector<uint32_t>&& data) {
    indices = std::move(data);
    return *this;
}

Mesh& Mesh::SetLayout(const VertexLayout& layout_) {
    layout = layout_;
    return *this;
}

Mesh& Mesh::SetPrimitiveType(const PrimitiveType type) {
    primitiveType = type;
    return *this;
}

void Mesh::SetupVertexAttributes() const {
    const GLsizei stride = static_cast<GLsizei>(layout.GetStride());
    size_t offset = 0;

    for (size_t i = 0; i < layout.attributes.size(); ++i) {
        const auto& attr = layout.attributes[i];

        glEnableVertexAttribArray(static_cast<GLuint>(i));

        if (attr.type == GL_FLOAT) {
            glVertexAttribPointer(static_cast<GLuint>(i), attr.count, attr.type,
                                  attr.normalized ? GL_TRUE : GL_FALSE, stride,
                                  reinterpret_cast<void*>(offset));
        } else {
            glVertexAttribIPointer(static_cast<GLuint>(i), attr.count, attr.type, stride,
                                   reinterpret_cast<void*>(offset));
        }

        offset += attr.GetSize();
    }
}

Mesh& Mesh::Upload() {
    if (vao == GL_NONE) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        if (!indices.empty()) {
            glGenBuffers(1, &ibo);
        }
    }

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(),
                     GL_STATIC_DRAW);
    }


    SetupVertexAttributes();

    glBindVertexArray(0);

    return *this;
}

void Mesh::Draw() const {
    if (vao == GL_NONE)
        return;

    glBindVertexArray(vao);

    if (!indices.empty()) {
        glDrawElements(static_cast<GLenum>(primitiveType), static_cast<GLsizei>(indices.size()),
                       GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(static_cast<GLenum>(primitiveType), 0, static_cast<GLsizei>(GetVertexCount()));
    }

    glBindVertexArray(0);
}

Mesh Mesh::PlanarGrid(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max) {
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    const float dx = 1.0f / static_cast<float>(size.x - 1);
    const float dy = 1.0f / static_cast<float>(size.y - 1);

    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            // X,Y pos
            vertices.push_back(min.x + (max.x - min.x) * x * dx);
            vertices.push_back(min.y + (max.y - min.y) * y * dy);
            // UV
            vertices.push_back(x * dx);
            vertices.push_back(y * dy);
        }
    }

    for (uint32_t y = 0; y < size.y - 1; y++) {
        for (uint32_t x = 0; x < size.x - 1; x++) {
            auto i0 = y * size.x + x;
            auto i1 = y * size.x + (x + 1);
            auto i2 = (y + 1) * size.x + x;
            auto i3 = (y + 1) * size.x + (x + 1);

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i2);
        }
    }

    return std::move(Mesh()
                         .SetVertices(vertices)
                         .SetIndices(indices)
                         .SetLayout({{GL_FLOAT, 2}, {GL_FLOAT, 2}})
                         .Upload());
}

size_t Mesh::GetVertexCount() const {
    const size_t stride = layout.GetStride();
    return stride > 0 ? (vertices.size() * sizeof(float)) / stride : 0;
}

void Mesh::Cleanup() {
    if (vao != GL_NONE) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        if (ibo != GL_NONE) {
            glDeleteBuffers(1, &ibo);
        }
        vao = vbo = ibo = GL_NONE;
    }
}
