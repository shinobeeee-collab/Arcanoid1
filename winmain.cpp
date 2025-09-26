#include "Window.h"
#include "Engine.h"
#include "Timer.h"
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>

int WINAPI WinMain
(
    HINSTANCE hInst,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    Window wnd(800, 600, hInst);
    Engine engine(wnd);

    Timer ft;
    const float FPS_LIMIT = 120.0f;
    const float FRAME_TIME = 1.0f / FPS_LIMIT;
    float accumulator = 0.0f;
    std::string winName = "Arcanoid Game ";

    while (wnd.RunMessageLoop())
    {
        float dt = ft.Go();
        accumulator += dt;

        // Ограничение FPS
        if (accumulator < FRAME_TIME)
        {
            std::this_thread::sleep_for(std::chrono::duration<float>(FRAME_TIME - accumulator));
            dt += FRAME_TIME - accumulator;
        }
        accumulator = 0.0f;

        // Замер времени обработки кадра
        auto start = std::chrono::steady_clock::now();

        engine.Run(wnd);

        auto end = std::chrono::steady_clock::now();

        std::chrono::duration<float> frameDuration = end - start;
        float fps = 1.0f / dt;

        // Вывод FPS и времени кадра в заголовок окна
        std::ostringstream titleStream;
        titleStream << winName << static_cast<int>(fps);
        SetWindowTextA(wnd.GetCustomWindow(), titleStream.str().c_str());

        // Вывод времени обработки кадра в Output
        std::ostringstream debugStream;
        debugStream << "Frame processing time: "
            << std::fixed << std::setprecision(4)
            << frameDuration.count() << " s\n";
        OutputDebugStringA(debugStream.str().c_str());
    }

    return 0;
}
