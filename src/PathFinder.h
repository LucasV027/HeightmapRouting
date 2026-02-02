#pragma once

#include <vector>

#include "Curve.h"
#include "Mat.h"

class PathFinder {
public:
    using Metric = std::pair<Curve, Mat<float>>;
    using Path = std::vector<glm::vec2>;

    enum class Connectivity { C4 = 4, C8 = 8 };

    PathFinder() = default;

    PathFinder& From(int x, int y);
    PathFinder& To(int x, int y);
    PathFinder& With(const Curve& curve, const Mat<float>& values);
    PathFinder& SetConnectivity(Connectivity c);

    Path Compute();

private:
    glm::ivec2 start = {-1, -1};
    glm::ivec2 end = {-1, -1};
    Connectivity connectivity = Connectivity::C4;
    std::vector<Metric> metrics;
};
