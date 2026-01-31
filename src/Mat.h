#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include <glm/glm.hpp>

#include "Image.h"

template <typename T>
class Mat {
public:
    explicit Mat(const glm::uvec2& size,
                 const glm::vec2& min = {0.0f, 0.0f},
                 const glm::vec2& max = {1.0f, 1.0f}) :
        size(size), min(min), delta((max - min) / glm::vec2(size - 1u)), data(size.x * size.y) {}

    explicit Mat(const Image& img,
                 const glm::vec2& min = {0.0f, 0.0f},
                 const glm::vec2& max = {1.0f, 1.0f}) :
        size(img.width, img.height), min(min), delta((max - min) / glm::vec2(size - 1u)),
        data(size.x * size.y) {
        for (uint32_t i = 0; i < data.size(); i++)
            data[i] = static_cast<T>(img.data[i]) / static_cast<T>(255);
    }

    Mat(const glm::uvec2& size,
        const glm::vec2& min,
        const glm::vec2& max,
        const std::function<T(const glm::vec2&)>& f) : Mat(size, min, max) {
        for (uint32_t y = 0; y < size.y; y++)
            for (uint32_t x = 0; x < size.x; x++)
                data[Index(x, y)] = f(Point(x, y));
    }

    uint32_t Width() const { return size.x; }
    uint32_t Height() const { return size.y; }
    const glm::uvec2& Size() const { return size; }

    T* Data() { return data.data(); }
    const T* Data() const { return data.data(); }

    const T& operator()(const uint32_t x, const uint32_t y) const { return data[Index(x, y)]; }
    T& operator()(const uint32_t x, const uint32_t y) { return data[Index(x, y)]; }

private:
    uint32_t Index(const uint32_t x, const uint32_t y) const { return y * size.x + x; }

    glm::vec2 Point(const uint32_t x, const uint32_t y) const {
        return min + glm::vec2(x, y) * delta;
    }

private:
    glm::uvec2 size;
    glm::vec2 min, delta;
    std::vector<T> data;
};
