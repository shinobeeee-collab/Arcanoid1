#pragma once
#include "Vec2.h"

class RectF
{
public:
    float left, right, top, bottom;

    RectF() = default;
    RectF(float left_in, float right_in, float top_in, float bottom_in);
    RectF(const Vec2& topLeft, const Vec2& bottomRight);
    RectF(const Vec2& topLeft, float width, float height);

    static RectF FromCenter(const Vec2& center, float halfWidth, float halfHeight);
    bool IsOverlappingWith(const RectF& other) const;
};
