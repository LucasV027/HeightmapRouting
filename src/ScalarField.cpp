#include "ScalarField.h"

#include <fstream>

bool Triangulation::SaveOBJ(const std::filesystem::path& path) {
    std::ofstream out(path);
    if (!out) return false;

    for (const auto& v : vertices)
        out << "v " << v.x << " " << v.y << " " << v.z << "\n";

    for (const auto& i : indices)
        out << "f " << i.x + 1 << " " << i.y + 1 << " " << i.z + 1 << "\n";

    return true;
}

ScalarField::ScalarField(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max) :
    size(size),
    min(min),
    max(max),
    delta((max - min) / glm::vec2(size - 1u)),
    heights(size.x * size.y) {}

ScalarField::ScalarField(const Image& img, const glm::vec2& min, const glm::vec2& max, const float maxHeight) :
    ScalarField(
        glm::uvec2(img.width, img.height), min, max) {
    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            heights[Index(x, y)] = img.data[Index(x, y)] / 255.0f * maxHeight;
        }
    }
}

ScalarField::ScalarField(const glm::uvec2& size,
                         const glm::vec2& min,
                         const glm::vec2& max,
                         const std::function<float(const glm::vec2&)>& f) : ScalarField(size, min, max) {
    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            heights[Index(x, y)] = f(Point(x, y));
        }
    }
}

Triangulation ScalarField::Triangulate() const {
    Triangulation tri;
    tri.vertices.reserve(size.x * size.y);
    tri.indices.reserve((size.x - 1) * (size.y - 1) * 2);

    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            glm::vec2 p = Point(x, y);
            float h = heights[Index(x, y)];
            tri.vertices.emplace_back(p.x, h, p.y);
        }
    }

    for (uint32_t y = 0; y < size.y - 1; y++) {
        for (uint32_t x = 0; x < size.x - 1; x++) {
            auto i0 = Index(x, y);
            auto i1 = Index(x + 1, y);
            auto i2 = Index(x, y + 1);
            auto i3 = Index(x + 1, y + 1);

            tri.indices.emplace_back(i0, i1, i2);
            tri.indices.emplace_back(i1, i2, i3);
        }
    }

    return tri;
}

uint32_t ScalarField::Index(const uint32_t x, const uint32_t y) const {
    return y * size.x + x;
}

glm::vec2 ScalarField::Point(const uint32_t x, const uint32_t y) const {
    return min + glm::vec2(x, y) * delta;
}
