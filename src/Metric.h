#pragma once

#include "Mat.h"
#include "PathFinder.h"

namespace Metric {
    PathFinder::CostFunction Slope(const Mat<float>& heightMap, float scale);
    PathFinder::CostFunction Distance();
} // namespace Metric
