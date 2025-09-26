#include "RectF.h"

RectF::RectF(float left_in, float right_in, float top_in, float bottom_in)
    : left(left_in), right(right_in), top(top_in), bottom(bottom_in) {
}

RectF::RectF(const Vec2& topLeft, const Vec2& bottomRight)
    : RectF(topLeft.x, bottomRight.x, topLeft.y, bottomRight.y) {
}

RectF::RectF(const Vec2& topLeft, float width, float height)
    : RectF(topLeft, topLeft + Vec2(width, height)) {
}

RectF RectF::FromCenter(const Vec2& center, float halfWidth, float halfHeight)
{
    return RectF(center.x - halfWidth, center.x + halfWidth, center.y - halfHeight, center.y + halfHeight);
}

bool RectF::IsOverlappingWith(const RectF& other) const
{
    return right > other.left && left < other.right && bottom > other.top && top < other.bottom;
}
