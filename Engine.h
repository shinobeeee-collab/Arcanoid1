#pragma once
#include "Window.h"
#include "Graphics.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>

class Engine
{
public:
    Engine(Window& wnd);
    ~Engine();

    void Run(Window& wnd);

private:
    void Update(Window& wnd, float dt);
    void ComposeFrame(Window& wnd);
    void InitBricks();

private:
    Graphics gfx;
    int* Colors;

    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;

    float fpsTimer;
    int frameCount;

    bool aKeyPrevState = false;//ƒл€ изменени€ направлени€ м€ча
};
