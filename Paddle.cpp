#include "Paddle.h"

Paddle::Paddle(const Vec2& centerPos, float w, float h)
    : pos(centerPos), width(w), height(h), speed(500.0f)
{
}

void Paddle::Update(float dt, bool moveLeft, bool moveRight, float screenWidth)
{
    if (moveLeft) pos.x -= speed * dt;
    if (moveRight) pos.x += speed * dt;
    if (pos.x - width / 2 < 0) pos.x = width / 2;
    if (pos.x + width / 2 > screenWidth) pos.x = screenWidth - width / 2;
}

RectF Paddle::GetRect() const
{
    return RectF::FromCenter(pos, width / 2, height / 2);
}

void Paddle::Draw(int* Colors, Graphics& gfx)
{
    Vec2 topLeft = pos - Vec2(width / 2, height / 2);
    for (int y = 0; y < int(height); ++y)
        for (int x = 0; x < int(width); ++x)
            gfx.DrawPixel(Colors, int(topLeft.x) + x, int(topLeft.y) + y, 0, 0, 255);
}
