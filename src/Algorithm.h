#pragma once

#include "Mat.h"

namespace Algorithm {

    Mat<glm::vec3> NormalMap(const Mat<float>& heights, float scale);

    Mat<glm::vec2> Gradient(const Mat<float>& in);

} // namespace Algorithm
