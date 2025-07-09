#include "core/engine.hpp"

namespace drop::core
{
    Engine::Engine()
        : _window {1600, 900, "Drop Engine"},
          _timer {} { }

    i32 Engine::Run()
    {
        while (true)
        {
            if (const auto exitCode {platform::Window::ProcessMessages()})
            {
                return *exitCode;
            }

            DoFrame();
        }
    }

    void Engine::DoFrame()
    {
        const f32 c {static_cast<f32>(sin(_timer.Peek()) / 2.0f + 0.5f)};
        _window.GetGraphics().ClearBuffer(c, c, 1.0f);
        _window.GetGraphics().DrawTestTriangle();
        _window.GetGraphics().EndFrame();
    }

} // namespace drop::core