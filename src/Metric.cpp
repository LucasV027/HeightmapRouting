#include "Metric.h"

static constexpr float SQRT_2 = 1.41421356f;
static constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

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

PathFinder::CostFunction Metric::Distance() {
    return [](const PathFinder::Edge& e) -> float {
        constexpr float MAX_DIST = SQRT_2;
        return e.d / MAX_DIST;
    };
}

PathFinder::CostFunction Metric::Terrain(const Mat<Terrain::TileType>& typeMap) {
    return [typeMap](const PathFinder::Edge& e) -> float {
        const auto t1 = typeMap(e.x1, e.y1);
        const auto t2 = typeMap(e.x2, e.y2);
        // This is a road
        if (!e.isBridgeCandidate) {
            if (t1 == Terrain::TileType::WATER || t2 == Terrain::TileType::WATER) {
                return MAX_FLOAT;
            }
            if (t1 == Terrain::TileType::FOREST || t2 == Terrain::TileType::FOREST) {
                return 10.f;
            }
        } else {
            return e.d * 20.f; // Bridge cost
        }
        return 0.0f;
    };
}
