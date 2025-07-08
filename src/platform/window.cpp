#include "platform/window.hpp"
#include "utils/resource.h"

#include <sstream>

namespace drop::platform
{
    // Window class singleton.
    Window::WindowClass Window::WindowClass::s_wndClass;

    Window::WindowClass::WindowClass() noexcept
        : _hInst {GetModuleHandleA(nullptr)}
    {
        D_ASSERT(_hInst, "GetModuleHandleA() failed.");

        WNDCLASSEXA wc {};
        wc.cbSize        = sizeof(WNDCLASSEXW);
        wc.hInstance     = GetInstance();
        wc.style         = CS_OWNDC;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hCursor       = static_cast<HCURSOR>(LoadImageA(_hInst, MAKEINTRESOURCEA(IDC_POINTER), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE));
        wc.hIcon         = static_cast<HICON>(LoadImageA(_hInst, MAKEINTRESOURCEA(IDI_ICON), IMAGE_ICON, 32, 32, 0));
        wc.hIconSm       = static_cast<HICON>(LoadImageA(_hInst, MAKEINTRESOURCEA(IDI_ICON), IMAGE_ICON, 16, 16, 0));
        wc.hbrBackground = nullptr;
        wc.lpszMenuName  = nullptr;
        wc.lpszClassName = GetName();
        wc.lpfnWndProc   = HandleMsgSetup;

        if (!RegisterClassExA(&wc))
        {
            D_ASSERT(false, "RegisterClassExA() failed.");
        }
    }

    Window::WindowClass::~WindowClass()
    {
        UnregisterClassA(GetName(), GetInstance());
    }

    const char* Window::WindowClass::GetName() noexcept
    {
        return s_WND_CLASS_NAME;
    }

    HINSTANCE Window::WindowClass::GetInstance() noexcept
    {
        return s_wndClass._hInst;
    }

    // Window class.
    Window::Window(i32 width, i32 height, const char* title)
        : _width(width), _height(height)
    {
        DWORD dwStyle {WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX};

        RECT rc {0, 0, width, height};
        if (!AdjustWindowRectEx(&rc, dwStyle, FALSE, 0))
        {
            throw CHWND_LAST_EXCEPT();
        }

        _hwnd = CreateWindowExA(
            0,
            WindowClass::GetName(), title,
            dwStyle,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rc.right - rc.left, rc.bottom - rc.top,
            nullptr, nullptr,
            WindowClass::GetInstance(),
            this);

        if (!_hwnd)
        {
            throw CHWND_LAST_EXCEPT();
        }

        ShowWindow(_hwnd, SW_SHOW);
        UpdateWindow(_hwnd);
    }

    Window::~Window()
    {
        DestroyWindow(_hwnd);
    }

    LRESULT CALLBACK Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        // Basically this proc is only for handling the first call msg WM_NCCREATE.
        // If it's done than we change the msg proc to the HandleMsgThunk one (normal).
        // So in WM_NCCREATE we need to pass in the instance of the window class that get created
        // to user data using SetWindowLongPtr so we can get it back in HandleMsgThunk.
        // This method will allow us to have different callback behaviors for different windows.
        if (msg == WM_NCCREATE)
        {
            // Extract pointer to window class that we passed in during CreateWindowEx.
            const CREATESTRUCTA* const pCreate {reinterpret_cast<CREATESTRUCTA*>(lParam)};
            Window* const              pWnd {static_cast<Window*>(pCreate->lpCreateParams)};
            // Set WINAPI-managed user data to store pointer to window class.
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
            // Set msg proc to normal (non-setup) handler now that setup is finished.
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
            // Forward message to window class handler.
            return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
        }

        // Just return to default proc if we get other msg before WM_NCCREATE.
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK Window::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        // Basically this proc is just the entry point for all messages that get from the window.
        // All we need to do here is just forward the right message to their respective window class handler.
        Window* const pWnd {reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))};
        return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
    }

    LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
    {
        switch (msg)
        {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_KILLFOCUS:
        {
            keyboard.ClearState();
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            if (!(lParam & 0x40000000) || keyboard.IsAutoRepeatEnabled())
            {
                keyboard.OnKeyPressed(static_cast<u8>(wParam));
            }
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            keyboard.OnKeyReleased(static_cast<u8>(wParam));
            break;
        }
        case WM_CHAR:
        {
            keyboard.OnChar(static_cast<u8>(wParam));
            break;
        }
        default:
            break;
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    // Window exception.
    Window::Exception::Exception(i32 line, const char* file, HRESULT hr) noexcept
        : DropException {line, file}, _hr {hr} { }

    const char* Window::Exception::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Error Code] " << GetErrorCode() << std::endl
            << "[Description] " << GetErrorString() << std::endl
            << GetOriginString();

        _whatBuffer = oss.str();
        return _whatBuffer.c_str();
    }

    const char* Window::Exception::GetType() const noexcept
    {
        return "Drop Window Exception";
    }

    std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
    {
        char* pMsgBuf {nullptr};
        DWORD nMsgLen {FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr)};

        if (!nMsgLen)
        {
            return "Unidentified error code";
        }

        std::string errorString {pMsgBuf};
        LocalFree(pMsgBuf);
        return errorString;
    }

    HRESULT Window::Exception::GetErrorCode() const noexcept
    {
        return _hr;
    }

    std::string Window::Exception::GetErrorString() const noexcept
    {
        return TranslateErrorCode(_hr);
    }
}