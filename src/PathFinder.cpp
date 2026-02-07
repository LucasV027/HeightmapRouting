#include "PathFinder.h"

#include <algorithm>
#include <random>
#include <utility>

#include <glm/ext/scalar_constants.hpp>


PathFinder::Edge::Edge(
    const int x1, const int y1, const int x2, const int y2, const bool bridgeCandidate) :
    x1(x1), y1(y1), x2(x2), y2(y2), d(std::hypotf(x2 - x1, y2 - y1)),
    isBridgeCandidate(bridgeCandidate) {
}

PathFinder& PathFinder::From(const int x, const int y) {
    start.x = x;
    start.y = y;
    return *this;
}

PathFinder& PathFinder::To(const int x, const int y) {
    end.x = x;
    end.y = y;
    return *this;
}

PathFinder& PathFinder::Size(const int x, const int y) {
    size.x = x;
    size.y = y;
    return *this;
}

PathFinder& PathFinder::With(float weight, const CostFunction& f) {
    metrics.emplace_back(weight, f);
    return *this;
}

PathFinder& PathFinder::SetConnectivity(const Connectivity c) {
    connectivity = c;
    return *this;
}

PathFinder& PathFinder::AllowBridges(const bool allow) {
    allowBridges = allow;
    return *this;
}

PathFinder::Path PathFinder::Compute() {
    if (!Validate())
        return {};

    Mat<float> costs(size, std::numeric_limits<float>::infinity());
    Mat<int> parent(size, -1);
    PriorityQueue pq;

    // Init
    costs(start.x, start.y) = 0.0f;
    pq.push({start.x, start.y, 0.0f});

    // Directions:
    int dx[8];
    int dy[8];

    // clang-format off
    if (connectivity == Connectivity::C4) {
        dx[0] = 00; dy[0] = -1;
        dx[1] = 00; dy[1] = 01;
        dx[2] = -1; dy[2] = 00;
        dx[3] = 01; dy[3] = 00;
    } else if (connectivity == Connectivity::C8) {
        dx[0] = 00; dy[0] = 01;
        dx[1] = 00; dy[1] = -1;
        dx[2] = 01; dy[2] = 00;
        dx[3] = -1; dy[3] = 00;
        dx[4] = 01; dy[4] = 01;
        dx[5] = 01; dy[5] = -1;
        dx[6] = -1; dy[6] = 01;
        dx[7] = -1; dy[7] = -1;
    } else {
        std::unreachable();
    }
    // clang-format on

    std::vector<Edge> bridgeCandidates;
    if (allowBridges) {
        bridgeCandidates = GenerateBridgeCandidates();
    }

    while (!pq.empty()) {
        auto [cx, cy, currentCost] = pq.top();
        pq.pop();

        // Found the destination
        if (cx == end.x && cy == end.y)
            break;

        // Skip if we've already found a better path
        if (currentCost > costs(cx, cy))
            continue;

        const int parentIdx = Index(cx, cy);

        // Explore neighbors (C4/C8 roads)
        for (int i = 0; i < static_cast<int>(connectivity); i++) {
            const int nx = cx + dx[i];
            const int ny = cy + dy[i];
            const auto edge = Edge(cx, cy, nx, ny, false);

            ProcessEdge(edge, currentCost, costs, parent, pq, parentIdx);
        }

        // Explore bridge candidates
        if (allowBridges) {
            for (const auto& bridge : bridgeCandidates) {
                if (bridge.x1 == cx && bridge.y1 == cy) {
                    ProcessEdge(bridge, currentCost, costs, parent, pq, parentIdx);
                }
            }
        }
    }

    // No path found
    if (std::isinf(costs(end.x, end.y)))
        return {};

    // Path reconstruction
    Path path;
    glm::ivec2 it(end.x, end.y);
    while (it.x != start.x || it.y != start.y) {
        path.points.emplace_back(it);
        const int p = parent(it.x, it.y);

        const int px = p % size.x;
        const int py = p / size.x;
        it.x = px;
        it.y = py;
    }
    path.points.emplace_back(start);
    std::ranges::reverse(path.points);
    path.cost = costs(end.x, end.y);

    return path;
}

bool PathFinder::Validate() const {
    return (start.x >= 0 && start.y >= 0 && end.x >= 0 && end.y >= 0) &&
        (size.x > 0 && size.y > 0) &&
        (start.x < size.x && start.y < size.y && end.x < size.x && end.y < size.y) &&
        !metrics.empty();
}

bool PathFinder::InBounds(const int x, const int y) const {
    return x >= 0 && x < size.x && y >= 0 && y < size.y;
}

int PathFinder::Index(const int x, const int y) const {
    return y * size.x + x;
}

std::vector<PathFinder::Edge> PathFinder::GenerateBridgeCandidates() const {
    std::vector<Edge> candidates;

    thread_local std::mt19937 rng(std::random_device{}());

    // TODO: Make this configurable
    constexpr int minBridgeLength = 50;
    constexpr int maxBridgeLength = 200;
    const int numSamples = std::max(100, (size.x * size.y) / 100);

    std::uniform_int_distribution<int> distX(0, size.x - 1);
    std::uniform_int_distribution<int> distY(0, size.y - 1);
    std::uniform_int_distribution<int> distLen(minBridgeLength, maxBridgeLength);

    for (int i = 0; i < numSamples; i++) {
        int x1 = distX(rng);
        int y1 = distY(rng);
        const int length = distLen(rng);

        for (int dir = 0; dir < 8; dir++) {
            const float angle = dir * 2.0f * glm::pi<float>() / 8.0f;
            int x2 = x1 + static_cast<int>(length * std::cos(angle));
            int y2 = y1 + static_cast<int>(length * std::sin(angle));

            if (InBounds(x2, y2)) {
                candidates.emplace_back(x1, y1, x2, y2, true);
                candidates.emplace_back(x2, y2, x1, y1, true);
            }
        }
    }

    return candidates;
}

void PathFinder::ProcessEdge(const Edge& edge,
                             const float currentCost,
                             Mat<float>& costs,
                             Mat<int>& parent,
                             std::priority_queue<Node, std::vector<Node>, std::greater<>>& pq,
                             const int parentIndex) {
    const int nx = edge.x2;
    const int ny = edge.y2;

    if (!InBounds(nx, ny))
        return;

    // Calculate edge cost
    float edgeCost = 0.0f;
    for (const auto& [weight, costFunction] : metrics) {
        edgeCost += weight * costFunction(edge);
    }

    if (std::isinf(edgeCost))
        return;

    const float newCost = currentCost + edgeCost;

    // Update if better path found
    if (newCost < costs(nx, ny)) {
        costs(nx, ny) = newCost;
        parent(nx, ny) = parentIndex;
        pq.push({nx, ny, newCost});
    }
}
