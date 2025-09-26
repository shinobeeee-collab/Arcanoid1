#include "Engine.h"
#include <windows.h>
#include <algorithm>
#include <string>

static float Clamp(float val, float minVal, float maxVal)
{
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

Engine::Engine(Window& wnd)
    : Colors(wnd.GetColorBuffer()),
    ball(Vec2(400.0f, 300.0f), Vec2(200.0f, -200.0f)),
    paddle(Vec2(400.0f, 550.0f), 200.0f, 20.0f),
    fpsTimer(0.0f),
    frameCount(0)
{
    InitBricks();
}

Engine::~Engine() {}

void Engine::InitBricks()
{
    bricks.clear();
    const int rows = 5;
    const int cols = 10;
    const float brickWidth = 60.0f;
    const float brickHeight = 20.0f;
    const float offsetX = 35.0f;
    const float offsetY = 50.0f;

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
        {
            float x = offsetX + c * (brickWidth + 5);
            float y = offsetY + r * (brickHeight + 5);
            bricks.emplace_back(Vec2(x, y), brickWidth, brickHeight,
                200, 50 + r * 30, 50 + c * 20);
        }
}

void Engine::Run(Window& wnd)
{
    float dt = 0.016f; // фиксированный шаг, если нет FrameTimer
    frameCount++;
    fpsTimer += dt;

    if (fpsTimer >= 1.0f)
    {
        int fps = frameCount;
        std::string title = "Арканоид - FPS: " + std::to_string(fps);
        SetWindowTextA(wnd.GetCustomWindow(), title.c_str());
        frameCount = 0;
        fpsTimer = 0.0f;
    }

    Update(wnd, dt);
    ComposeFrame(wnd);

    RECT rc;
    GetClientRect(wnd.GetCustomWindow(), &rc);
    HDC hdc = GetDC(wnd.GetCustomWindow());
    wnd.UpdateWindowBuffer(hdc, rc.right - rc.left, rc.bottom - rc.top);
    ReleaseDC(wnd.GetCustomWindow(), hdc);
}

void Engine::Update(Window& wnd, float dt)
{
    // Двигаем платформу
    paddle.Update(dt, wnd.kbd.KeyIsPressed(VK_LEFT), wnd.kbd.KeyIsPressed(VK_RIGHT), 800.0f);

    if (wnd.kbd.KeyIsPressed(VK_SHIFT))
    {
        // Управление мячом мышью
        float mx = Clamp(wnd.mouse.GetPos().x, Ball::radius, 800.0f - Ball::radius);
        float my = Clamp(wnd.mouse.GetPos().y, Ball::radius, 600.0f - Ball::radius);
        ball.SetPos(Vec2(mx, my));

        // Рассчитываем прогноз траектории
        ball.PredictTrajectory(paddle, bricks, 800.0f, 600.0f);
    }
    else
    {
        // Обычное движение мяча
        ball.Update(dt);

        // Коллизия с платформой
        if (ball.GetRect().IsOverlappingWith(paddle.GetRect()))
        {
            ball.ReboundY();
            ball.pos.y = paddle.pos.y - paddle.height / 2 - Ball::radius;
        }

        for (auto& brick : bricks)
        {
            if (ball.BounceOffBrick(brick))
                break;
        }


        // Коллизия со стенами
        if (ball.pos.x - Ball::radius < 0.0f) { ball.pos.x = Ball::radius; ball.ReboundX(); }
        if (ball.pos.x + Ball::radius > 800.0f) { ball.pos.x = 800.0f - Ball::radius; ball.ReboundX(); }
        if (ball.pos.y - Ball::radius < 0.0f) { ball.pos.y = Ball::radius; ball.ReboundY(); }
        if (ball.pos.y + Ball::radius > 600.0f) { ball.pos.y = 600.0f - Ball::radius; ball.ReboundY(); }
    }
}

void Engine::ComposeFrame(Window& wnd)
{
    // Очистка экрана
    gfx.ClearScreenSuperFast(Colors);

    // Рисуем кирпичи
    for (auto& brick : bricks)
        brick.Draw(Colors, gfx);

    // Рисуем платформу
    paddle.Draw(Colors, gfx);

    // Рисуем мяч
    ball.Draw(gfx, Colors, 800, 600);

    // Рисуем прогноз траектории, если зажат Shift
    if (wnd.kbd.KeyIsPressed(VK_SHIFT))
        ball.DrawTrajectory(gfx, Colors);
}
