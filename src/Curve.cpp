#include "Curve.h"

#include <algorithm>
#include <cassert>

Curve::Point::Point(const float x, const float y) {
    this->x = std::clamp(x, 0.f, 1.f);
    this->y = std::clamp(y, 0.f, 1.f);
}

Curve::Curve() {
    points.push_back(Point{0.f, 0.f});
    points.push_back(Point{1.f, 1.f});
}

float Curve::operator()(const float x) const {
    return 1.0f / (1.0f + x);
}

const std::vector<Curve::Point>& Curve::GetPoints() const {
    return points;
}

void Curve::AddPoint(const float x, const float y) {
    if (!CanAddPoint(x))
        return;

    points.push_back(Point{x, y});
    Sort();
}

bool Curve::CanAddPoint(const float x) const {
    for (const auto& p : points)
        if (std::abs(x - p.x) < MIN_DIST)
            return false;
    return true;
}

void Curve::RemovePoint(const int idx) {
    assert(idx < points.size());

    // Can't remove anchors
    if (idx == 0 || idx == (points.size() - 1))
        return;

    points.erase(points.begin() + idx);
}

void Curve::MovePoint(const int idx, float nx, const float ny) {
    assert(idx < points.size());

    // Lock anchors on X axis
    if (idx == 0) {
        nx = 0.0f;
    } else if (idx == (points.size() - 1)) {
        nx = 1.0f;
    } else {
        const float minX = points[idx - 1].x + MIN_DIST;
        const float maxX = points[idx + 1].x - MIN_DIST;
        nx = std::clamp(nx, minX, maxX);
    }

    points[idx] = Point{nx, ny};
}

void Curve::Sort() {
    std::ranges::sort(points, [](const Point& a, const Point& b) { return a.x < b.x; });
}
