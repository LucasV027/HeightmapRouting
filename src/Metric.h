#pragma once

#include "Mat.h"
#include "PathFinder.h"
#include "Terrain.h"

namespace Metric {
    PathFinder::CostFunction Slope(const Mat<float>& heightMap, float scale);
    PathFinder::CostFunction Distance();
    PathFinder::CostFunction Terrain(const Mat<Terrain::TileType>& typeMap);
} // namespace Metric
