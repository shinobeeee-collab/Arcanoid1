#include "Brick.h"

Brick::Brick(const Vec2& topLeft, float w, float h, unsigned char r, unsigned char g, unsigned char b)
    : pos(topLeft), width(w), height(h), R(r), G(g), B(b), destroyed(false)
{
}

RectF Brick::GetRect() const
{
    return RectF(pos.x, pos.x + width, pos.y, pos.y + height);
}

void Brick::Draw(int* Colors, Graphics& gfx)
{
    if (destroyed) return;
    for (int y = int(pos.y); y < int(pos.y + height); ++y)
        for (int x = int(pos.x); x < int(pos.x + width); ++x)
            gfx.DrawPixel(Colors, x, y, R, G, B);
}
