#include "core/engine.hpp"

#include "drawable/box.hpp"

namespace drop::core
{
    Engine::Engine()
        : _window {1600, 900, "Drop Engine"},
          _timer {}
    {
        std::mt19937                        rng {std::random_device {}()};
        std::uniform_real_distribution<f32> adist {0.0f, 3.1415f * 2.0f};
        std::uniform_real_distribution<f32> ddist {0.0f, 3.1415f * 2.0f};
        std::uniform_real_distribution<f32> odist {0.0f, 3.1415f * 0.3f};
        std::uniform_real_distribution<f32> rdist {6.0f, 20.0f};

        for (i32 i {0}; i < 80; i++)
        {
            _boxes.push_back(std::make_unique<drawable::Box>(
                _window.GetGraphics(), rng,
                adist, ddist, odist, rdist));
        }

        _window.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(
            1.0f, 9.0f / 16.0f,
            0.5f, 40.0f));
    }

    Engine::~Engine() { }

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
        f32 dt {_timer.Mark()};
        _window.GetGraphics().ClearBuffer(0.0f, 1.0f, 1.0f);
        for (auto& b : _boxes)
        {
            b->Update(dt);
            b->Draw(_window.GetGraphics());
        }

        _window.GetGraphics().EndFrame();
    }

} // namespace drop::core