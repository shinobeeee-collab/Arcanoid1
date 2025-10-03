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

    float overlapX = min(b.right - r.left, r.right - b.left);
    float overlapY = min(b.bottom - r.top, r.bottom - b.top);

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


RectF Ball::MakeRect(const Vec2& center) const
{
    return RectF(
        center.x - radius, center.x + radius,
        center.y - radius, center.y + radius
    );
}

// ------------------------ “–ј≈ “ќ–»я ------------------------
void Ball::PredictTrajectory(const Paddle& paddle, const std::vector<Brick>& bricks,
    float screenWidth, float screenHeight)
{
    trajectoryPoints.clear();

    // --- копи€ позиции и скорости ---
    Vec2 tempPos = pos;
    Vec2 tempVel = vel;
    float tempRadius = radius;

    const float stepSize = 1.0f;   // маленький шаг
    const int maxSteps = 25;      // ограничение по количеству шагов

    for (int step = 0; step < maxSteps; ++step)
    {
        // движение виртуального шара
        tempPos = tempPos + tempVel * stepSize * 0.016f;

        // === стены ===
        if (tempPos.x - radius <= 0.0f)
        {
            tempPos.x = radius;
            tempVel.x = fabsf(tempVel.x);
        }
        if (tempPos.x + radius >= 785.0f)
        {
            tempPos.x = screenWidth - radius;
            tempVel.x = -fabsf(tempVel.x);
        }
        if (tempPos.y - radius <= 0.0f)
        {
            tempPos.y = radius;
            tempVel.y = fabsf(tempVel.y);
        }
        if (tempPos.y + radius >= 565.0f)
        {
            tempPos.y = screenHeight - radius;
            tempVel.y = -fabsf(tempVel.y);
        }

        // === пр€моугольник виртуального шара через MakeRect ===
        RectF tempRect = MakeRect(tempPos);

        // === платформа ===
        if (tempRect.IsOverlappingWith(paddle.GetRect()))
        {
            tempVel.y = -fabsf(tempVel.y);
            tempPos.y = paddle.GetRect().top - radius;
            tempRect = MakeRect(tempPos); // обновл€ем rect после изменени€ позиции
        }

        // === кирпичи ===
        for (const auto& brick : bricks)
        {
            if (brick.destroyed) continue;

            if (tempRect.IsOverlappingWith(brick.GetRect()))
            {
                float overlapLeft = (tempPos.x + radius) - brick.GetRect().left;
                float overlapRight = brick.GetRect().right - (tempPos.x - radius);
                float overlapTop = (tempPos.y + radius) - brick.GetRect().top;
                float overlapBottom = brick.GetRect().bottom - (tempPos.y - radius);

                float minOverlapX = min(overlapLeft, overlapRight);
                float minOverlapY = min(overlapTop, overlapBottom);

                if (minOverlapX < minOverlapY)
                {
                    tempVel.x = -tempVel.x;
                    if (overlapLeft < overlapRight)
                        tempPos.x = brick.GetRect().left - radius;
                    else
                        tempPos.x = brick.GetRect().right + radius;
                }
                else
                {
                    tempVel.y = -tempVel.y;
                    if (overlapTop < overlapBottom)
                        tempPos.y = brick.GetRect().top - radius;
                    else
                        tempPos.y = brick.GetRect().bottom + radius;
                }

                // обновл€ем rect после изменени€ позиции
                tempRect = MakeRect(tempPos);
            }
        }

        // сохран€ем точку в траекторию
        trajectoryPoints.push_back(tempPos);
    }
}

void Ball::DrawTrajectory(Graphics& gfx, int* Colors)
{
    for (auto& p : trajectoryPoints)
        gfx.DrawPixel(Colors, int(p.x), int(p.y), 255, 255, 255); 
}

// ------------------------ –исуем м€ч ------------------------
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
void Ball::CheckPolySphereCollision(Ball& ball, Paddle& paddle)
{
}