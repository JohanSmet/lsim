// algebra.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic algebra classes - only implements things used in lsim

#ifndef LSIM_GUI_ALGEBRA_H
#define LSIM_GUI_ALGEBRA_H

#include <array>

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

public:
    float x;
    float y;
};

class Transform {
public:
    Transform();
    Transform(const Transform &other);

    void reset();
    void rotate(float degrees);
    void translate(const Point &delta);

    Point apply(const Point &p) const;

private:
    float m_el[3][2];       // 3 columns, 2 rows
};

#endif // LSIM_GUI_ALGEBRA_H