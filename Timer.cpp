#include "Timer.h"

Timer::Timer() { last = std::chrono::steady_clock::now(); }

float Timer::Go()
{
    auto old = last;
    last = std::chrono::steady_clock::now();
    std::chrono::duration<float> dt = last - old;
    return dt.count();
}
