#pragma once

#include <functional>
#include <vector>

#include "Mat.h"

class PathFinder {
public:
    struct Edge {
        int x1, y1;
        int x2, y2;
        float d;

        Edge(int x1, int y1, int x2, int y2);
    };

    struct Path {
        std::vector<glm::vec2> points;
        float cost = -1.0f;

        operator bool() const { return cost != -1.0f; }
    };

    using CostFunction = std::function<float(Edge)>;
    struct Metric {
        float weight;
        CostFunction cost;
    };

    enum class Connectivity { C4 = 4, C8 = 8 };

    PathFinder() = default;

    PathFinder& From(int x, int y);
    PathFinder& To(int x, int y);
    PathFinder& Size(int x, int y);
    PathFinder& With(float weight, const CostFunction& f);
    PathFinder& SetConnectivity(Connectivity c);

    Path Compute();

private:
    bool Validate() const;
    bool InBounds(int x, int y) const;


private:
    glm::ivec2 start = {-1, -1};
    glm::ivec2 end = {-1, -1};
    glm::ivec2 size = {-1, -1};
    Connectivity connectivity = Connectivity::C4;
    std::vector<Metric> metrics;
};
