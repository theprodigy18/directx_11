#pragma once

#include <chrono>

namespace drop::utils
{
    class Timer
    {
    public:
        Timer();
        f32 Mark();
        f32 Peek() const;

    private:
        std::chrono::steady_clock::time_point _last;
    };
} // namespace drop::utils