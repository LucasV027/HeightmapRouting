#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Image.h"

template <typename T>
class Mat {
public:
    explicit Mat(const glm::uvec2& size) : size(size), data(size.x * size.y) {}
    Mat(const glm::uvec2& size, const T& value) : size(size), data(size.x * size.y, value) {}

    explicit Mat(const Image& img) : size(img.width, img.height), data(size.x * size.y) {
        assert(img.format == Image::Format::I);
        for (uint32_t i = 0; i < data.size(); i++)
            data[i] = img.data[i] / 255.f;
    }

    uint32_t Width() const { return size.x; }
    uint32_t Height() const { return size.y; }
    const glm::uvec2& Size() const { return size; }

    T* Data() { return data.data(); }
    const T* Data() const { return data.data(); }

    const T& operator()(const uint32_t x, const uint32_t y) const { return data[Index(x, y)]; }
    T& operator()(const uint32_t x, const uint32_t y) { return data[Index(x, y)]; }

    uint32_t Index(const uint32_t x, const uint32_t y) const { return y * size.x + x; }

protected:
    glm::uvec2 size;
    std::vector<T> data;
};
