#pragma once
#include "Vec2.h"
#include "RectF.h"
#include "Graphics.h"

class Paddle
{
public:
    Vec2 pos;
    float width;
    float height;
    float speed;

    Paddle(const Vec2& centerPos, float w, float h);

    void Update(float dt, bool moveLeft, bool moveRight, float screenWidth);
    RectF GetRect() const;
    void Draw(int* Colors, Graphics& gfx);
};
