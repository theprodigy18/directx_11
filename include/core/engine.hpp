#pragma once

#include "platform/window.hpp"
#include "utils/timer.hpp"

#include <memory>

namespace drop::drawable
{
    class Box;
} // namespace drop::drawable

namespace drop::core
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        i32 Run();

    private:
        void DoFrame();

        platform::Window                            _window;
        utils::Timer                                _timer;
        std::vector<std::unique_ptr<drawable::Box>> _boxes;
    };
} // namespace drop::core