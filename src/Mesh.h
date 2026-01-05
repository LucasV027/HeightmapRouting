#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Window.h"

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw() const;

    static Mesh PlanarGrid(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max);

private:
    void Cleanup();

private:
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    GLuint vao = GL_NONE;
    GLuint vbo = GL_NONE;
    GLuint ibo = GL_NONE;
};
