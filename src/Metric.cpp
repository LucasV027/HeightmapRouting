#include "Metric.h"

PathFinder::CostFunction Metric::Slope(const Mat<float>& heightMap, float scale) {
    return [heightMap, scale](const PathFinder::Edge& e) -> float {
        const float h1 = heightMap(e.x1, e.y1) * scale;
        const float h2 = heightMap(e.x2, e.y2) * scale;
        const float dh = std::abs(h2 - h1);

        const float slope = dh / e.d;

        constexpr float MAX_SLOPE = 1.0f;
        return std::clamp(slope / MAX_SLOPE, 0.0f, 1.0f);
    };
}

PathFinder::CostFunction Metric:: Distance() {
    return [](const PathFinder::Edge& e) -> float {
        constexpr float MAX_DIST = 1.41421356f; // sqrt(2)
        return e.d / MAX_DIST;
    };
}
