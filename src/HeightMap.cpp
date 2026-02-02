#include "HeightMap.h"

Mat<float> HeightMap::Procedural(const glm::uvec2& size,
                                 const glm::vec2& min,
                                 const glm::vec2& max,
                                 const std::function<float(const glm::vec2&)>& f) {
    Mat<float> hm(size);
    const auto delta = (max - min) / glm::vec2(size - 1u);

    for (uint32_t y = 0; y < size.y; y++) {
        for (uint32_t x = 0; x < size.x; x++) {
            glm::vec2 p = min + glm::vec2(x, y) * delta;
            hm(x, y) = f(p);
        }
    }
    return hm;
}
