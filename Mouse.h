#pragma once
#include "Vec2.h"

class Mouse
{
public:
    void SetPos(int x, int y) { pos.x = float(x); pos.y = float(y); }
    Vec2 GetPos() const { return pos; }
private:
    Vec2 pos{ 0.0f,0.0f };
};
