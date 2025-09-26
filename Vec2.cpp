#include "Vec2.h"

Vec2::Vec2(float x_in, float y_in) : x(x_in), y(y_in) {}

Vec2 Vec2::operator+(const Vec2& rhs) const { return Vec2(x + rhs.x, y + rhs.y); }
Vec2& Vec2::operator+=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
Vec2 Vec2::operator-(const Vec2& rhs) const { return Vec2(x - rhs.x, y - rhs.y); }
Vec2& Vec2::operator-=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
Vec2 Vec2::operator*(float rhs) const { return Vec2(x * rhs, y * rhs); }
Vec2& Vec2::operator*=(float rhs) { x *= rhs; y *= rhs; return *this; }
float Vec2::GetLength() const { return std::sqrt(x * x + y * y); }
float Vec2::GetLengthSq() const { return x * x + y * y; }
Vec2& Vec2::Normalize() { float len = GetLength(); if (len != 0) { x /= len; y /= len; } return *this; }
Vec2 Vec2::GetNormalized() const { float len = GetLength(); return (len != 0) ? Vec2(x / len, y / len) : Vec2(0, 0); }
