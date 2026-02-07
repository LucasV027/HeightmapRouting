#pragma once

#include <functional>
#include <queue>
#include <vector>

#include "Mat.h"

class PathFinder {
public:
    struct Node {
        int x, y;
        float cost;

        bool operator>(const Node& other) const { return cost > other.cost; }
    };

    struct Edge {
        int x1, y1;
        int x2, y2;
        float d;
        bool isBridgeCandidate;

        Edge(int x1, int y1, int x2, int y2, bool bridgeCandidate = false);
    };

    struct Path {
        std::vector<glm::vec2> points;
        float cost = -1.0f;

        operator bool() const { return cost != -1.0f; }
    };

    using CostFunction = std::function<float(Edge)>;
    using PriorityQueue = std::priority_queue<Node, std::vector<Node>, std::greater<>>;

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
    PathFinder& AllowBridges(bool allow);

    Path Compute();

private:
    bool Validate() const;
    bool InBounds(int x, int y) const;
    int Index(int x, int y) const;

    std::vector<Edge> GenerateBridgeCandidates() const;
    void ProcessEdge(const Edge& edge,
                     float currentCost,
                     Mat<float>& costs,
                     Mat<int>& parent,
                     PriorityQueue& pq,
                     int parentIndex);

private:
    bool allowBridges = false;
    glm::ivec2 start = {-1, -1};
    glm::ivec2 end = {-1, -1};
    glm::ivec2 size = {-1, -1};
    Connectivity connectivity = Connectivity::C4;
    std::vector<Metric> metrics;
};
