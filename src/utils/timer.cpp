#include "utils/timer.hpp"

namespace drop::utils
{

    Timer::Timer() { _last = std::chrono::steady_clock::now(); }

    f32 Timer::Mark()
    {
        const auto old {_last};
        _last = std::chrono::steady_clock::now();
        const std::chrono::duration<f32> frameTime {_last - old};
        return frameTime.count();
    }

    f32 Timer::Peek() const
    {
        return std::chrono::duration<f32>(std::chrono::steady_clock::now() - _last).count();
    }

} // namespace drop::utils