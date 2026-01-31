#pragma once

#include <vector>

class Curve {
public:
    struct Point {
        float x, y;

        Point(float x, float y);
    };

public:
    Curve();

    void AddPoint(float x, float y);
    bool CanAddPoint(float x) const;
    void RemovePoint(int idx);
    void MovePoint(int idx, float nx, float ny);

    float operator()(float x) const;
    const std::vector<Point>& GetPoints() const;

private:
    void Sort();

private:
    static constexpr float MIN_DIST = 0.01f;

    std::vector<Point> points;
};
