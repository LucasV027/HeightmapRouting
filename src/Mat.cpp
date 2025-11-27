#include "Mat.h"

#include <stdexcept>
#include <algorithm>
#include <cassert>

Mat::Mat(const uint32_t w, const uint32_t h, const float v) : width(w), height(h), data(w * h, v) {}

Mat::Mat(const Mat& other) : width(other.width), height(other.height), data(other.data) {}

Mat::Mat(Mat&& other) noexcept : width(other.width), height(other.height), data(std::move(other.data)) {
    other.width = 0;
    other.height = 0;
}

Mat& Mat::operator=(const Mat& other) {
    if (this != &other) {
        width = other.width;
        height = other.height;
        data = other.data;
    }
    return *this;
}

Mat& Mat::operator=(Mat&& other) noexcept {
    if (this != &other) {
        width = other.width;
        height = other.height;
        data = std::move(other.data);

        other.width = 0;
        other.height = 0;
    }
    return *this;
}

static void CheckSameDimensions(const Mat& a, const Mat& b) {
    if (a.Width() != b.Width() || a.Height() != b.Height()) {
        throw std::invalid_argument("Matrices must have the same dimensions.");
    }
}

Mat& Mat::operator+=(const Mat& other) {
    CheckSameDimensions(*this, other);
    for (int i = 0; i < width * height; i++)
        data[i] += other.data[i];
    return *this;
}

Mat& Mat::operator-=(const Mat& other) {
    CheckSameDimensions(*this, other);
    for (int i = 0; i < width * height; i++)
        data[i] -= other.data[i];
    return *this;
}

Mat& Mat::operator*=(const float scalar) {
    for (int i = 0; i < width * height; i++)
        data[i] *= scalar;
    return *this;
}

Mat& Mat::operator/=(const float scalar) {
    for (int i = 0; i < width * height; i++)
        data[i] /= scalar;
    return *this;
}

Mat operator+(const Mat& lhs, const Mat& rhs) {
    Mat m(lhs);
    return m += rhs;
}

Mat operator-(const Mat& lhs, const Mat& rhs) {
    Mat m(lhs);
    return m -= rhs;
}

Mat operator*(const float scalar, const Mat& mat) {
    Mat m(mat);
    m *= scalar;
    return m;
}

Mat Mat::operator/(const float scalar) const {
    Mat m(*this);
    m /= scalar;
    return m;
}

float Mat::Max() const {
    assert(width > 0 && height > 0);
    return *std::ranges::max_element(data);
}

float Mat::Min() const {
    assert(width > 0 && height > 0);
    return *std::ranges::min_element(data);
}

Mat Mat::Clamp(const Mat& mat, const float min, const float max) {
    Mat m(mat);
    for (float& v : m.data)
        v = std::clamp(v, min, max);
    return m;
}

Mat Mat::Normalize(const Mat& mat) {
    Mat m(mat);
    if (const float max = mat.Max(); max > 0.0f)
        m /= max;
    return m;
}

Mat Mat::Convolve(const Mat& mat, const glm::mat3& kernel) {
    Mat m(mat.width, mat.height, 0.0f);

    for (uint32_t y  = 1; y < mat.height - 1; y++) {
        for (uint32_t x = 1; x < mat.width - 1; x++) {
            uint32_t i = m.Index(x,y);
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
            m.data[i] += kernel[0][0] * 0.0f;
        }
    }

    return m;




}

uint32_t Mat::Index(const uint32_t x, const uint32_t y) const {
    return y * width + x;
}
