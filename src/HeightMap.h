#pragma once

#include <functional>

#include "Mat.h"

namespace HeightMap {
    Mat<float> Procedural(const glm::uvec2& size,
                          const glm::vec2& min,
                          const glm::vec2& max,
                          const std::function<float(const glm::vec2&)>& f);
}
