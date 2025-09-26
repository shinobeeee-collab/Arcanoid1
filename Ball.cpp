#include "Ball.h"
#include <cmath>

Ball::Ball(const Vec2& p, const Vec2& v)
    : pos(p), vel(v)
{
}

void Ball::SetPos(const Vec2& p)
{
    pos = p;
}

bool Ball::BounceOffBrick(Brick& brick)
{
    if (brick.destroyed) return false;

    if (!GetRect().IsOverlappingWith(brick.GetRect()))
        return false;

    auto b = GetRect();
    auto r = brick.GetRect();

    float overlapX = std::min(b.right - r.left, r.right - b.left);
    float overlapY = std::min(b.bottom - r.top, r.bottom - b.top);

    if (overlapX < overlapY) ReboundX();
    else ReboundY();

    brick.destroyed = true;
    return true;
}


RectF Ball::GetRect() const
{
    return RectF(pos.x - radius, pos.x + radius, pos.y - radius, pos.y + radius);
}

void Ball::ReboundX() { vel.x = -vel.x; }
void Ball::ReboundY() { vel.y = -vel.y; }

void Ball::Update(float dt)
{
    pos = pos + vel * dt;
}

// ------------------------ ТРАЕКТОРИЯ ------------------------
void Ball::PredictTrajectory(const Paddle& paddle, const std::vector<Brick>& bricks, float screenWidth, float screenHeight)
{
    trajectoryPoints.clear();
    Vec2 tempPos = pos;
    Vec2 tempVel = vel;

    for (int i = 0; i < 200; ++i) // максимум шагов прогнозирования
    {
        tempPos = tempPos + tempVel * 0.016f; // небольшой dt

        // Стены
        if (tempPos.x - radius < 0) { tempPos.x = radius; tempVel.x = -tempVel.x; }
        if (tempPos.x + radius > screenWidth) { tempPos.x = screenWidth - radius; tempVel.x = -tempVel.x; }
        if (tempPos.y - radius < 0) { tempPos.y = radius; tempVel.y = -tempVel.y; }
        if (tempPos.y + radius > screenHeight) { tempPos.y = screenHeight - radius; tempVel.y = -tempVel.y; }

        // Платформа
        RectF paddleRect = paddle.GetRect();
        RectF ballRect(tempPos.x - radius, tempPos.x + radius, tempPos.y - radius, tempPos.y + radius);
        if (ballRect.IsOverlappingWith(paddleRect))
        {
            tempVel.y = -tempVel.y;
            tempPos.y = paddleRect.top - radius; // чтобы не застрял
        }

        // Кирпичи
        for (const auto& brick : bricks)
        {
            if (brick.destroyed) continue;
            RectF brickRect = brick.GetRect();
            if (ballRect.IsOverlappingWith(brickRect))
            {
                float overlapLeft = tempPos.x + radius - brickRect.left;
                float overlapRight = brickRect.right - (tempPos.x - radius);
                float overlapTop = tempPos.y + radius - brickRect.top;
                float overlapBottom = brickRect.bottom - (tempPos.y - radius);

                bool ballFromLeft = overlapLeft < overlapRight;
                bool ballFromTop = overlapTop < overlapBottom;

                float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
                float minOverlapY = ballFromTop ? overlapTop : overlapBottom;

                if (minOverlapX < minOverlapY) tempVel.x = -tempVel.x;
                else tempVel.y = -tempVel.y;
            }
        }

        trajectoryPoints.push_back(tempPos);
    }
}

void Ball::DrawTrajectory(Graphics& gfx, int* Colors)
{
    for (auto& p : trajectoryPoints)
        gfx.DrawPixel(Colors, int(p.x), int(p.y), 123, 255, 0); 
}

// ------------------------ Рисуем мяч ------------------------
void Ball::Draw(Graphics& gfx, int* Colors, float screenWidth, float screenHeight)
{
    for (int y = int(pos.y - radius); y <= int(pos.y + radius); ++y)
    {
        for (int x = int(pos.x - radius); x <= int(pos.x + radius); ++x)
        {
            float dx = x - pos.x;
            float dy = y - pos.y;
            if (dx * dx + dy * dy <= radius * radius)
                gfx.DrawPixel(Colors, x, y, 255, 0, 0);
        }
    }
}
