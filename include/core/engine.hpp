#pragma once

#include "platform/window.hpp"
#include "utils/timer.hpp"

namespace drop::core
{
    class Engine
    {
    public:
        Engine();

        i32 Run();

    private:
        void DoFrame();

        platform::Window _window;
        utils::Timer     _timer;
    };
} // namespace drop::core