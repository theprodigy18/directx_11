#pragma once

#include "utils/drop_exception.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "graphics/graphics.hpp"

#include <Windows.h>
#include <optional>

namespace drop::platform
{
    class Window
    {
    public:
        class Exception : public utils::DropException
        {
        public:
            Exception(i32 line, const char* file, HRESULT hr) noexcept;
            const char*        what() const noexcept override;
            const char*        GetType() const noexcept override;
            static std::string TranslateErrorCode(HRESULT hr) noexcept;
            HRESULT            GetErrorCode() const noexcept;
            std::string        GetErrorString() const noexcept;

        private:
            HRESULT _hr;
        };

    private:
        class WindowClass // Singleton Class.
        {
        public:
            static const char* GetName() noexcept;
            static HINSTANCE   GetInstance() noexcept;

        private:
            WindowClass() noexcept;
            ~WindowClass();
            WindowClass(const WindowClass&)            = delete; // Copy Constructor.
            WindowClass& operator=(const WindowClass&) = delete; // Copy Assignment Operator.

            static constexpr const char* s_WND_CLASS_NAME {"DropWindowClass"};
            static WindowClass           s_wndClass;
            HINSTANCE                    _hInst {nullptr};
        };

    public:
        Window(i32 width, i32 height, const char* title);
        ~Window();
        Window(const Window&)            = delete; // Copy Constructor.
        Window& operator=(const Window&) = delete; // Copy Assignment Operator.

        void                      SetTitle(const char* title);
        static std::optional<i32> ProcessMessages();
        graphics::Graphics&       GetGraphics();

        input::Keyboard keyboard;
        input::Mouse    mouse;

    private:
        static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        LRESULT                 HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

        i32                 _width {0};
        i32                 _height {0};
        HWND                _hwnd {nullptr};
        graphics::Graphics* _graphics {nullptr};
    };

} // namespace drop::platform

#define CHWND_EXCEPT(hr) drop::platform::Window::Exception(__LINE__, __FILE__, hr)
#define CHWND_LAST_EXCEPT() drop::platform::Window::Exception(__LINE__, __FILE__, GetLastError())
