#include <windows.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>

// ------------------ утилиты ------------------
inline float myMin(float a, float b) { return (a < b) ? a : b; }
inline float myMax(float a, float b) { return (a > b) ? a : b; }
float Clamp(float val, float minVal, float maxVal) { return myMax(minVal, myMin(maxVal, val)); }

struct Vec2 {
    float x, y;
    Vec2(float a = 0, float b = 0) : x(a), y(b) {}
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
};

static Vec2 Normalize(const Vec2& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len <= 1e-6f) return Vec2(1.0f, 0.0f);
    return Vec2(v.x / len, v.y / len);
}

// ------------------ структуры ------------------
struct RectF {
    float left, right, top, bottom;
    RectF(float l = 0, float r = 0, float t = 0, float b = 0) : left(l), right(r), top(t), bottom(b) {}
    bool Overlaps(const RectF& o) const { return right > o.left && left < o.right && bottom > o.top && top < o.bottom; }
};

struct Brick {
    Vec2 pos; float width, height; int R, G, B; bool destroyed;
};

struct Paddle {
    Vec2 pos; float width, height; float speed;
};

struct Ball {
    Vec2 pos; float radius;
    float speed; // дл€ предсказани€
    float angle; // направление движени€ (в радианах)
    std::vector<Vec2> trajectory;

    void PredictTrajectory(const Vec2& startPos, float startSpeed, float startAngle, const Paddle& paddleRef, const std::vector<Brick>& bricksRef, int screenWidth, int screenHeight);
};

// ------------------ глобальные переменные ------------------
int* Colors = nullptr;
int screenWidth = 800;
int screenHeight = 600;
Ball ball;
Paddle paddle;
std::vector<Brick> bricks;

// ------------------ функции рисовани€ ------------------
void DrawPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
        Colors[y * screenWidth + x] = (r << 16) | (g << 8) | b;
}

void ClearScreen() {
    for (int i = 0; i < screenWidth * screenHeight; i++) Colors[i] = 0;
}

void DrawBallAt(const Vec2& p, float radius, unsigned char r, unsigned char g, unsigned char b) {
    int top = int(p.y - radius);
    int bottom = int(p.y + radius);
    int left = int(p.x - radius);
    int right = int(p.x + radius);
    for (int y = top; y <= bottom; ++y)
        for (int x = left; x <= right; ++x) {
            float dx = x + 0.5f - p.x;
            float dy = y + 0.5f - p.y;
            if (dx * dx + dy * dy <= radius * radius) DrawPixel(x, y, r, g, b);
        }
}

void DrawBall() {
    DrawBallAt(ball.pos, ball.radius, 255, 0, 0);
}

void DrawPaddle() {
    int left = int(paddle.pos.x - paddle.width / 2);
    int top = int(paddle.pos.y - paddle.height / 2);
    for (int y = 0; y < int(paddle.height); y++)
        for (int x = 0; x < int(paddle.width); x++)
            DrawPixel(left + x, top + y, 0, 0, 255);
}

void DrawBricks() {
    for (auto& b : bricks) {
        if (b.destroyed) continue;
        int top = int(b.pos.y);
        int bottom = int(b.pos.y + b.height);
        int left = int(b.pos.x);
        int right = int(b.pos.x + b.width);
        for (int y = top; y < bottom; ++y)
            for (int x = left; x < right; ++x)
                DrawPixel(x, y, (unsigned char)b.R, (unsigned char)b.G, (unsigned char)b.B);
    }
}

void DrawTrajectory() {
    // белые точки по пути
    for (auto& p : ball.trajectory)
        DrawPixel(int(p.x), int(p.y), 255, 255, 255);

    // зелЄный м€ч в конце пути
    if (!ball.trajectory.empty())
        DrawBallAt(ball.trajectory.back(), ball.radius, 0, 255, 0);
}

// ------------------ инициализаци€ ------------------
void InitBricks() {
    bricks.clear();
    int rows = 5, cols = 10;
    float bw = 60, bh = 20, ox = 35, oy = 50;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
        {
            Brick b;
            b.pos = Vec2(ox + c * (bw + 5), oy + r * (bh + 5));
            b.width = bw; b.height = bh;
            b.R = 200; b.G = 50 + r * 30; b.B = 50 + c * 20;
            b.destroyed = false;
            bricks.push_back(b);
        }
}

// ------------------ физика ------------------
RectF GetBallRect(const Vec2& pos, float radius) { return RectF(pos.x - radius, pos.x + radius, pos.y - radius, pos.y + radius); }
RectF GetPaddleRect() { return RectF(paddle.pos.x - paddle.width / 2, paddle.pos.x + paddle.width / 2, paddle.pos.y - paddle.height / 2, paddle.pos.y + paddle.height / 2); }

void BallBounce(Vec2& pos, float& angle, float radius, Paddle& paddleRef, std::vector<Brick>& bricksRef) {
    // движение по стенкам
    if (pos.x - radius < 0) { pos.x = radius; angle = 3.1415926f - angle; }
    if (pos.x + radius > screenWidth) { pos.x = screenWidth - radius; angle = 3.1415926f - angle; }
    if (pos.y - radius < 0) { pos.y = radius; angle = -angle; }
    if (pos.y + radius > screenHeight) { pos.y = screenHeight - radius; angle = -angle; }

    RectF ballRect = GetBallRect(pos, radius);

    // платформа
    if (ballRect.Overlaps(GetPaddleRect())) {
        angle = -angle;
        pos.y = GetPaddleRect().top - radius;
    }

    // кирпичи
    for (auto& br : bricksRef) {
        if (br.destroyed) continue;
        float closestX = Clamp(pos.x, br.pos.x, br.pos.x + br.width);
        float closestY = Clamp(pos.y, br.pos.y, br.pos.y + br.height);
        float dx = pos.x - closestX;
        float dy = pos.y - closestY;
        if (dx * dx + dy * dy <= radius * radius) {
            if (std::abs(dx) > std::abs(dy)) angle = 3.1415926f - angle;
            else angle = -angle;
            br.destroyed = true;
            break;
        }
    }
}

// ------------------ предсказанна€ траектори€ ------------------
void Ball::PredictTrajectory(const Vec2& startPos, float startSpeed, float startAngle, const Paddle& paddleRef, const std::vector<Brick>& bricksRef, int screenWidth, int screenHeight) {
    trajectory.clear();
    Vec2 tempPos = startPos;
    float angle = startAngle;

    const float dt = 0.016f;
    const int maxSteps = 25;

    std::vector<Brick> bricksCopy = bricksRef; // временна€ копи€ кирпичей

    for (int i = 0; i < maxSteps; ++i) {
        tempPos.x += std::cos(angle) * startSpeed * dt;
        tempPos.y += std::sin(angle) * startSpeed * dt;

        BallBounce(tempPos, angle, radius, const_cast<Paddle&>(paddleRef), bricksCopy);

        trajectory.push_back(tempPos);
    }
}

// ------------------ WinMain ------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "ArcanoidClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowA("ArcanoidClass", "Arcanoid",
        WS_OVERLAPPEDWINDOW, 100, 100, screenWidth, screenHeight,
        nullptr, nullptr, hInst, nullptr);
    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screenWidth;
    bmi.bmiHeader.biHeight = -screenHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    Colors = new int[screenWidth * screenHeight];

    // инициализаци€ объектов
    ball.pos = Vec2(400, 300); ball.radius = 10; ball.speed = 300; ball.angle = -3.1415926f / 4;
    paddle.pos = Vec2(400, 550); paddle.width = 200; paddle.height = 20; paddle.speed = 500;

    InitBricks();

    auto lastTime = std::chrono::steady_clock::now();
    bool left = false, right = false;

    MSG msg;
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto end;
            TranslateMessage(&msg); DispatchMessage(&msg);
            if (msg.message == WM_KEYDOWN) { if (msg.wParam == VK_LEFT) left = true; if (msg.wParam == VK_RIGHT) right = true; }
            if (msg.message == WM_KEYUP) { if (msg.wParam == VK_LEFT) left = false; if (msg.wParam == VK_RIGHT) right = false; }
        }

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> dt = now - lastTime;
        lastTime = now;
        float delta = dt.count();

        // движение платформы
        if (left) paddle.pos.x -= paddle.speed * delta;
        if (right) paddle.pos.x += paddle.speed * delta;
        paddle.pos.x = Clamp(paddle.pos.x, paddle.width / 2, screenWidth - paddle.width / 2);

        // движение м€ча
        ball.pos.x += std::cos(ball.angle) * ball.speed * delta;
        ball.pos.y += std::sin(ball.angle) * ball.speed * delta;
        BallBounce(ball.pos, ball.angle, ball.radius, paddle, bricks);

        ClearScreen();
        DrawBricks();
        DrawPaddle();

        // --- траектори€ при зажатом SHIFT ---
        if ((GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
            POINT mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(hwnd, &mousePos);

            // перемещаем реальный м€ч к мыши
            ball.pos = Vec2(float(mousePos.x), float(mousePos.y));

            // строим предсказанную траекторию от текущей позиции и направлени€
            ball.PredictTrajectory(ball.pos, ball.speed, ball.angle, paddle, bricks, screenWidth, screenHeight);

            // рисуем траекторию
            DrawTrajectory();
        }


        DrawBall();

        HDC hdc = GetDC(hwnd);
        StretchDIBits(hdc, 0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, Colors, &bmi, DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(hwnd, hdc);

        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

end:
    delete[] Colors;
    return 0;
}
