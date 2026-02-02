#include "PathFinder.h"

#include <algorithm>
#include <queue>

struct Node {
    int x, y;
    float cost;

    bool operator>(const Node& other) const { return cost > other.cost; }
};

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

PathFinder& PathFinder::With(const Curve& curve, const Mat<float>& values) {
    metrics.emplace_back(curve, values);
    return *this;
}

PathFinder& PathFinder::SetConnectivity(const Connectivity c) {
    connectivity = c;
    return *this;
}

PathFinder::Path PathFinder::Compute() {
    // Validation
    if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
        return {};

    if (metrics.empty())
        return {};

    // Assume all metrics have the same dimensions
    const auto s = metrics.front().second.Size();

    // Bounds check
    if (start.x >= s.x || start.y >= s.y || end.x >= s.x || end.y >= s.y)
        return {};

    Mat<float> costs(s, std::numeric_limits<float>::infinity());
    Mat<uint8_t> parent(s, 0);
    std::priority_queue<Node, std::vector<Node>, std::greater<>> pq;

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

    while (!pq.empty()) {
        auto [cx, cy, currentCost] = pq.top();
        pq.pop();

        // Found the destination
        if (cx == end.x && cy == end.y)
            break;

        // Skip if we've already found a better path
        if (currentCost > costs(cx, cy))
            continue;

        // Explore neighbors
        for (int i = 0; i < static_cast<int>(connectivity); ++i) {
            const int nx = cx + dx[i];
            const int ny = cy + dy[i];

            // Bounds check
            if (nx < 0 || nx >= s.x || ny < 0 || ny >= s.y)
                continue;

            // Calculate edge cost (Average of all metrics)
            float edgeCost = 0.0f;
            for (const auto& [curve, values] : metrics) {
                const float vx = values(cx, cy);
                const float vy = values(nx, ny);
                const float dv = std::abs(vx - vy);

                edgeCost += curve(dv);
            }
            edgeCost /= metrics.size();

            const float newCost = currentCost + edgeCost;

            // Update if better path found
            if (newCost < costs(nx, ny)) {
                costs(nx, ny) = newCost;
                parent(nx, ny) = i;
                pq.push({nx, ny, newCost});
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
        path.emplace_back(it);
        const uint8_t p = parent(it.x, it.y);
        it -= glm::ivec2(dx[p], dy[p]);
    }
    path.emplace_back(start);
    std::ranges::reverse(path);

    return path;
}
