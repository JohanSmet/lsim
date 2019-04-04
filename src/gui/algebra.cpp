// algebra.cpp - Johan Smet - BSD-3-Clause (see LICENSE)
//
// basic algebra classes - only implements things used in lsim

#include "algebra.h"
#include <cstring>
#include <cmath>

constexpr const float PI = 3.14159265358979323846f;

inline float deg2rad(float deg) {
	return (deg * PI) / 180.0f;
}

///////////////////////////////////////////////////////////////////////////////
//
// Point
//

Point::Point() : x(0.0f), y(0.0f) {
}

Point::Point(float x, float y) : x(x), y(y) {
}

Point::Point(const Point &other) : x(other.x), y(other.y) {
}

Point::Point(const std::array<float, 2> a) : x(a[0]), y(a[1]) {

}

Point Point::operator+ (const Point &p) const {
    return Point(x + p.x, y + p.y);
}

Point Point::operator- (const Point &p) const {
    return Point(x - p.x, y - p.y);
}

Point Point::operator- () const {
    return Point(- x, - y);
}

Point Point::operator*(float scale) const {
    return Point(x * scale, y * scale);
}

Point Point::operator*(const Point &p) const {
    return Point(x * p.x, y * p.y);
}

Point Point::operator/(const Point &p) const {
    return Point(x / p.x, y / p.y);
}

///////////////////////////////////////////////////////////////////////////////
//
// Transform
//

Transform::Transform() {
    reset();
}

Transform::Transform(const Transform &other) {
    std::memcpy(m_el, other.m_el, sizeof(float) * 3 * 2);
}

void Transform::reset() {
    m_el[0][0] = 1.0f;  m_el[1][0] = 0.0f; m_el[2][0] = 0.0f;
    m_el[0][1] = 0.0f;  m_el[1][1] = 1.0f; m_el[2][1] = 0.0f;
}

void Transform::rotate(float degrees) {
    const float rad = deg2rad(degrees);
    const float sa = sinf(rad);
    const float ca = cosf(rad);
    const Transform cur(*this);

    m_el[0][0] = ca * cur.m_el[0][0] - sa * cur.m_el[0][1];
    m_el[1][0] = ca * cur.m_el[1][0] - sa * cur.m_el[1][1];
    m_el[2][0] = ca * cur.m_el[2][0] - sa * cur.m_el[2][1];

    m_el[0][1] = sa * cur.m_el[0][0] + ca * cur.m_el[0][1];
    m_el[1][1] = sa * cur.m_el[1][0] + ca * cur.m_el[1][1];
    m_el[2][1] = sa * cur.m_el[2][0] + ca * cur.m_el[2][1];
}

void Transform::translate(const Point &delta) {
    m_el[2][0] += delta.x;
    m_el[2][1] += delta.y;
}

Point Transform::apply(const Point &p) const {
    return Point(m_el[0][0] * p.x + m_el[1][0] * p.y + m_el[2][0],
                 m_el[0][1] * p.x + m_el[1][1] * p.y + m_el[2][1]);
}