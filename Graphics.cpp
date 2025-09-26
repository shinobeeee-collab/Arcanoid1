#include "Graphics.h"
#include <cassert>
#include <cmath>

Graphics::Graphics() {}
Graphics::~Graphics() {}

void Graphics::FillScreenFast(int* Colors, unsigned char r, unsigned char g, unsigned char b)
{
    for (int* i = Colors, *end = &Colors[800 * 600]; i < end; ++i)
        *i = (r << 16) | (g << 8) | b;
}

void Graphics::ClearScreenSuperFast(int* Colors)
{
    for (int* i = Colors, *end = &Colors[800 * 600]; i < end; ++i)
        *i = 0;
}

void Graphics::DrawPixel(int* Colors, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    //assert(x >= 0 && x < 800 && y >= 0 && y < 600);
    Colors[y * 800 + x] = (r << 16) | (g << 8) | b;
}

void Graphics::DrawCircle(int* Colors, int cx, int cy, int radius, unsigned char r, unsigned char g, unsigned char b)
{
    int r2 = radius * radius;
    for (int y = -radius; y <= radius; ++y)
        for (int x = -radius; x <= radius; ++x)
            if (x * x + y * y <= r2)
            {
                int px = cx + x, py = cy + y;
                if (px >= 0 && px < 800 && py >= 0 && py < 600)
                    Colors[py * 800 + px] = (r << 16) | (g << 8) | b;
            }
}

void Graphics::DrawLine(int* Colors, int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (true)
    {
        if (x0 >= 0 && x0 < 800 && y0 >= 0 && y0 < 600)
            Colors[y0 * 800 + x0] = (r << 16) | (g << 8) | b;

        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
