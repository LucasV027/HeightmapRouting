#include "Curve.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>
#include <utility>

Curve::Point::Point(const float x, const float y) {
    this->x = std::clamp(x, 0.f, 1.f);
    this->y = std::clamp(y, 0.f, 1.f);
}

Curve::Curve(const Interpolation mode) : mode(mode) {
    points.push_back(Point{0.f, 0.f});
    points.push_back(Point{1.f, 1.f});
}

float Curve::operator()(float x) const {
    x = std::clamp(x, 0.0f, 1.0f);
    switch (mode) {
    case Interpolation::LINEAR:
        return LinearInterpolation(x);
    case Interpolation::COSINUS:
        return CosinusInterpolation(x);
    }

    std::unreachable();
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

void Curve::SetInterpolationMode(const Interpolation newMode) {
    mode = newMode;
}

Curve::Interpolation Curve::GetInterpolationMode() const {
    return mode;
}

void Curve::Sort() {
    std::ranges::sort(points, [](const Point& a, const Point& b) { return a.x < b.x; });
}

std::pair<size_t, float> Curve::FindSegmentAndT(const float x) const {
    for (size_t i = 0; i < points.size() - 1; ++i) {
        if (x >= points[i].x && x <= points[i + 1].x) {
            const float t = (x - points[i].x) / (points[i + 1].x - points[i].x);
            return {i, t};
        }
    }
    return {points.size() - 1, 1.0f};
}

float Curve::LinearInterpolation(const float x) const {
    auto [i, t] = FindSegmentAndT(x);

    if (i >= points.size() - 1)
        return points.back().y;

    return points[i].y + t * (points[i + 1].y - points[i].y);
}

float Curve::CosinusInterpolation(const float x) const {
    auto [i, t] = FindSegmentAndT(x);

    if (i >= points.size() - 1)
        return points.back().y;

    const float smoothT = (1.0f - std::cos(t * std::numbers::pi_v<float>)) * 0.5f;
    return points[i].y + smoothT * (points[i + 1].y - points[i].y);
}
