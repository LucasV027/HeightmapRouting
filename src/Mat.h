#pragma once
#include <cstdint>
#include <vector>

#include "glm/glm.hpp"

/*
Gradient(i,j);
GradientNorm();
Laplacian();
*/

class Mat {
public:
    Mat(uint32_t w, uint32_t h, float v = 0.0f);

    Mat(const Mat& other);
    Mat(Mat&& other) noexcept;

    Mat& operator=(const Mat& other);
    Mat& operator=(Mat&& other) noexcept;

    Mat& operator+=(const Mat& other);
    Mat& operator-=(const Mat& other);
    Mat& operator*=(float scalar);
    Mat& operator/=(float scalar);

    friend Mat operator+(const Mat& lhs, const Mat& rhs);
    friend Mat operator-(const Mat& lhs, const Mat& rhs);
    friend Mat operator*(float scalar, const Mat& mat);

    Mat operator/(float scalar) const;

    uint32_t Width() const { return width; }
    uint32_t Height() const { return height; }

    float Max() const;
    float Min() const;

    static Mat Clamp(const Mat& mat, float min, float max);
    static Mat Normalize(const Mat& mat);
    static Mat Convolve(const Mat& mat, const glm::mat3& kernel);

private:
    uint32_t Index(uint32_t x, uint32_t y) const;

private:
    uint32_t width = 0, height = 0;
    std::vector<float> data;
};
