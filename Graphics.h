#pragma once

class Graphics
{
public:
    Graphics();
    ~Graphics();

    void FillScreenFast(int* Colors, unsigned char r, unsigned char g, unsigned char b);
    void ClearScreenSuperFast(int* Colors);
    void DrawPixel(int* Colors, int x, int y, unsigned char r, unsigned char g, unsigned char b);
    void DrawCircle(int* Colors, int cx, int cy, int radius, unsigned char r, unsigned char g, unsigned char b);
    void DrawLine(int* Colors, int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
};
