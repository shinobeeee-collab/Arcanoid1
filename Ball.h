#pragma once
#include "Vec2.h"
#include "RectF.h"
#include "Graphics.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>
#include "Window.h"

class Ball
{
public:
    Vec2 pos;
    Vec2 vel;
    static constexpr float radius = 10.0f;

    Ball(const Vec2& p, const Vec2& v);

    void Update(float dt);
    RectF MakeRect(const Vec2& center) const;
    RectF GetRect() const;
    void ReboundX();
    void ReboundY();
    void SetPos(const Vec2& p);
    bool BounceOffBrick(Brick& brick);

    void PredictTrajectory(const Paddle& paddle, const std::vector<Brick>& bricks, float screenWidth, float screenHeight);
    void DrawTrajectory(Graphics& gfx, int* Colors);

    void Draw(Graphics& gfx, int* Colors, float screenWidth, float screenHeight);

private:
    std::vector<Vec2> trajectoryPoints;
};
