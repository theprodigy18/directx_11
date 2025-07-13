#pragma once

#include "utils/drop_exception.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "graphics/graphics.hpp"

#include <Windows.h>
#include <optional>
#include <memory>

namespace drop::platform
{
    class Window
    {
    public:
        class Exception : public utils::DropException
        {
            using utils::DropException::DropException;

        public:
            static std::string TranslateErrorCode(HRESULT hr) noexcept;
        };
        class HrException : public Exception
        {
        public:
            HrException(i32 line, const char* file, HRESULT hr) noexcept;
            const char* what() const noexcept override;
            const char* GetType() const noexcept override;
            HRESULT     GetErrorCode() const noexcept;
            std::string GetErrorDescription() const noexcept;

        private:
            HRESULT _hr;
        };
        class NoGraphicsException : public Exception
        {
        public:
            using Exception::Exception;
            const char* GetType() const noexcept override;
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
        static std::optional<i32> ProcessMessages() noexcept;
        graphics::Graphics&       GetGraphics();

        input::Keyboard keyboard;
        input::Mouse    mouse;

    private:
        static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        LRESULT                 HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

        i32                                 _width {0};
        i32                                 _height {0};
        HWND                                _hwnd {nullptr};
        std::unique_ptr<graphics::Graphics> _pGraphics {nullptr};
    };

} // namespace drop::platform
