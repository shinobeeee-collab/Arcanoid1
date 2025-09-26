#pragma once
#include "Vec2.h"
#include "RectF.h"
#include "Graphics.h"

class Brick
{
public:
    Vec2 pos;
    float width;
    float height;
    unsigned char R, G, B;
    bool destroyed;

    Brick(const Vec2& topLeft, float w, float h, unsigned char r, unsigned char g, unsigned char b);

    RectF GetRect() const;
    void Draw(int* Colors, Graphics& gfx);
};
