#pragma once

#include <vector>

class Curve {
public:
    enum class Interpolation { LINEAR, COSINUS };

    struct Point {
        float x, y;

        Point(float x, float y);
    };

public:
    explicit Curve(Interpolation mode);

    void AddPoint(float x, float y);
    bool CanAddPoint(float x) const;
    void RemovePoint(int idx);
    void MovePoint(int idx, float nx, float ny);

    void SetInterpolationMode(Interpolation newMode);
    Interpolation GetInterpolationMode() const ;

    float operator()(float x) const;
    const std::vector<Point>& GetPoints() const;

private:
    void Sort();

    std::pair<size_t, float> FindSegmentAndT(float x) const;
    float LinearInterpolation(float x) const;
    float CosinusInterpolation(float x) const;

private:
    static constexpr float MIN_DIST = 0.01f;

    Interpolation mode;
    std::vector<Point> points;
};
