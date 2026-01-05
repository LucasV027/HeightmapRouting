#include "Mesh.h"

Mesh::~Mesh() {
    Cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept :
    vertices(std::move(other.vertices)), indices(std::move(other.indices)), vao(other.vao),
    vbo(other.vbo), ibo(other.ibo) {
    other.vao = GL_NONE;
    other.vbo = GL_NONE;
    other.ibo = GL_NONE;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Cleanup();

        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        vao = other.vao;
        vbo = other.vbo;
        ibo = other.ibo;

        other.vao = GL_NONE;
        other.vbo = GL_NONE;
        other.ibo = GL_NONE;
    }
    return *this;
}

void Mesh::Draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh Mesh::PlanarGrid(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max) {
    constexpr auto posCount = 2;
    constexpr auto uvCount = 2;

    Mesh m;
    m.vertices.reserve(size.x * size.y * (posCount + uvCount));
    m.indices.reserve((size.x - 1) * (size.y - 1) * 2 * 3);

    const float dx = 1.0f / static_cast<float>(size.x - 1);
    const float dy = 1.0f / static_cast<float>(size.y - 1);

    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            // X,Y pos
            m.vertices.push_back(min.x + (max.x - min.x) * x * dx);
            m.vertices.push_back(min.y + (max.y - min.y) * y * dy);
            // UV
            m.vertices.push_back(x * dx);
            m.vertices.push_back(y * dy);
        }
    }

    for (uint32_t y = 0; y < size.y - 1; y++) {
        for (uint32_t x = 0; x < size.x - 1; x++) {
            auto i0 = y * size.x + x;
            auto i1 = y * size.x + (x + 1);
            auto i2 = (y + 1) * size.x + x;
            auto i3 = (y + 1) * size.x + (x + 1);

            m.indices.push_back(i0);
            m.indices.push_back(i1);
            m.indices.push_back(i2);

            m.indices.push_back(i1);
            m.indices.push_back(i3);
            m.indices.push_back(i2);
        }
    }

    glCreateVertexArrays(1, &m.vao);

    glCreateBuffers(1, &m.vbo);
    glNamedBufferData(m.vbo, m.vertices.size() * sizeof(float), m.vertices.data(), GL_STATIC_DRAW);

    glCreateBuffers(1, &m.ibo);
    glNamedBufferData(m.ibo, m.indices.size() * sizeof(uint32_t), m.indices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(m.vao, 0, m.vbo, 0, (posCount + uvCount) * sizeof(float));
    glVertexArrayElementBuffer(m.vao, m.ibo);

    // Position (2 Floats | Location = 0)
    glEnableVertexArrayAttrib(m.vao, 0);
    glVertexArrayAttribFormat(m.vao, 0, posCount, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m.vao, 0, 0);

    // UVs (2 Floats | Location = 1)
    glEnableVertexArrayAttrib(m.vao, 1);
    glVertexArrayAttribFormat(m.vao, 1, uvCount, GL_FLOAT, GL_FALSE, posCount * sizeof(float));
    glVertexArrayAttribBinding(m.vao, 1, 0);

    return m;
}

void Mesh::Cleanup() {
    if (vao) {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);
        vao = vbo = ibo = GL_NONE;
    }
}
