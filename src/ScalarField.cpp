#include "ScalarField.h"

ScalarField::ScalarField(const glm::uvec2& size, const glm::vec2& min, const glm::vec2& max) :
    size(size), min(min), max(max), delta((max - min) / glm::vec2(size - 1u)),
    heights(size.x * size.y, 0.f) {
}

ScalarField::ScalarField(const Image& img, const glm::vec2& min, const glm::vec2& max) :
    ScalarField(glm::uvec2(img.width, img.height), min, max) {
    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            heights[Index(x, y)] = img.data[Index(x, y)] / 255.0f;
        }
    }
}

ScalarField::ScalarField(const glm::uvec2& size,
                         const glm::vec2& min,
                         const glm::vec2& max,
                         const std::function<float(const glm::vec2&)>& f) :
    ScalarField(size, min, max) {
    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            heights[Index(x, y)] = f(Point(x, y));
        }
    }
}

PlanarGridMesh ScalarField::GridMesh() const {
    PlanarGridMesh pgm;
    pgm.size = size;
    pgm.positions.reserve(size.x * size.y);
    pgm.uvs.reserve(size.x * size.y);
    pgm.indices.reserve((size.x - 1) * (size.y - 1) * 2);

    const float dx = 1.0f / float(size.x - 1);
    const float dy = 1.0f / float(size.y - 1);

    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            pgm.positions.emplace_back(Point(x, y));
            pgm.uvs.emplace_back(dx * x, dy * y);
        }
    }

    for (uint32_t y = 0; y < size.y - 1; y++) {
        for (uint32_t x = 0; x < size.x - 1; x++) {
            auto i0 = Index(x, y);
            auto i1 = Index(x + 1, y);
            auto i2 = Index(x, y + 1);
            auto i3 = Index(x + 1, y + 1);

            pgm.indices.emplace_back(i0, i1, i2);
            pgm.indices.emplace_back(i1, i2, i3);
        }
    }

    return pgm;
}

glm::vec2 ScalarField::Middle() const {
    return (min + max) / 2.f;
}

uint32_t ScalarField::Index(const uint32_t x, const uint32_t y) const {
    return y * size.x + x;
}

glm::vec2 ScalarField::Point(const uint32_t x, const uint32_t y) const {
    return min + glm::vec2(x, y) * delta;
}
