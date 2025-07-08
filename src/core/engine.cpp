#include "core/engine.hpp"

#include <iomanip>
#include <sstream>

namespace drop::core
{
    Engine::Engine()
        : _window {1280, 720, "Drop Engine"},
          _timer {} { }

    i32 Engine::Run()
    {
        while (true)
        {
            if (const auto exitCode {platform::Window::ProcessMessages()})
            {
                return *exitCode;
            }

            // DoFrame();
        }
    }

    void Engine::DoFrame()
    {
        const f32 t {_timer.Peek()};

        std::ostringstream oss;
        oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << " seconds.";

        _window.SetTitle(oss.str().c_str());
    }

} // namespace drop::core