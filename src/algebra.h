// algebra.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic algebra classes - only implements things used in lsim

#ifndef LSIM_GUI_ALGEBRA_H
#define LSIM_GUI_ALGEBRA_H

#include <array>
#include <cmath>

namespace lsim {

inline bool float_equal(float a, float b, float epsilon = FLT_EPSILON) {
	return fabs(a - b) <= fmaxf(fabs(a), fabs(b))* epsilon;
}

class Point {
public:
    // constructors
    Point();
    Point(float x, float y);
    Point(const Point &other);
    Point(const std::array<float, 2> a);

    // operators
    Point operator+(const Point &p) const;
    Point operator-(const Point &p) const;
    Point operator-() const;
    Point operator*(float scale) const;
	Point operator*(int scale) const;
    Point operator*(const Point &p) const;
    Point operator/(const Point &p) const;

public:
    float x;
    float y;
};

float distance_squared(const Point &p1, const Point &p2);

inline bool operator==(const Point &p1, const Point &p2) {
    return float_equal(p1.x, p2.x) && float_equal(p1.y, p2.y);
}

inline bool operator!=(const Point &p1, const Point &p2) {
    return !float_equal(p1.x, p2.x) || !float_equal(p1.y, p2.y);
}

inline bool points_colinear(const Point &p0, const Point &p1, const Point &p2) {
// return true if all three points are on the same line
    return fabs(((p1.x - p0.x) * (p2.y - p0.y)) - ((p2.x - p0.x)  * (p1.y - p0.y))) < 0.001f;
}

inline bool between(float a, float b, float v) {
// return true if v is between a and b
    if (a < b) {
        return (v >= a) && (v <= b);
    } else {
        return (v >= b) && (v <= a);
    }
}

inline bool point_on_line_segment(const Point &s0, const Point &s1, const Point &p) {
// return true if p is on the line-segment formed by s0 and s1
    return points_colinear(s0, s1, p) &&
           between(s0.x, s1.x, p.x) &&
           between(s0.y, s1.y, p.y);
}

class Transform {
public:
    Transform();
    Transform(const Transform &other);

    void reset();
    void rotate(float degrees);
    void translate(const Point &delta);

    Point apply(const Point &p) const;
    Point apply_to_vector(const Point &v) const;

private:
    float m_el[3][2];       // 3 columns, 2 rows
};

} // namespace lsim

#endif // LSIM_GUI_ALGEBRA_H