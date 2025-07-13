#include "platform/window.hpp"
#include "utils/resource.hpp"

#include <sstream>

#include "utils/windows_throw_macros.hpp"

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
        wc.hCursor       = static_cast<HCURSOR>(LoadImageA(_hInst, MAKEINTRESOURCEA(IDC_DROP_ARROW), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE));
        wc.hIcon         = static_cast<HICON>(LoadImageA(_hInst, MAKEINTRESOURCEA(IDI_DROP_ICON), IMAGE_ICON, 32, 32, 0));
        wc.hIconSm       = static_cast<HICON>(LoadImageA(_hInst, MAKEINTRESOURCEA(IDI_DROP_ICON), IMAGE_ICON, 16, 16, 0));
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
            throw HWND_LAST_EXCEPT();
        }
        i32 normWidth {rc.right - rc.left};
        i32 normHeight {rc.bottom - rc.top};

        RECT dr {};
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &dr, 0);
        i32 dw {dr.right - dr.left};
        i32 dh {dr.bottom - dr.top};

        i32 x {dr.left + (dw - normWidth) / 2};
        i32 y {dr.top + (dh - normHeight) / 2};

        _hwnd = CreateWindowExA(
            0,
            WindowClass::GetName(), title,
            dwStyle,
            x, y,
            normWidth, normHeight,
            nullptr, nullptr,
            WindowClass::GetInstance(),
            this);

        if (!_hwnd)
        {
            throw HWND_LAST_EXCEPT();
        }

        // Create graphics object.
        _pGraphics = std::make_unique<graphics::Graphics>(_hwnd);

        ShowWindow(_hwnd, SW_SHOW);
        UpdateWindow(_hwnd);
    }

    Window::~Window()
    {
        DestroyWindow(_hwnd);
    }

    void Window::SetTitle(const char* title)
    {
        if (!SetWindowTextA(_hwnd, title))
        {
            throw HWND_LAST_EXCEPT();
        }
    }

    std::optional<i32> Window::ProcessMessages() noexcept
    {
        MSG msg {};
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        return {};
    }

    graphics::Graphics& Window::GetGraphics()
    {
        if (!_pGraphics)
        {
            throw HWND_NOGFX_EXCEPT();
        }
        return *_pGraphics;
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
        case WM_MOUSEMOVE:
        {
            const POINTS pt {MAKEPOINTS(lParam)};
            // First check if the mouse is in the window, then we can update the mouse position.
            // In addition if before this message the mouse is not in the window, we can set the mouse in the window.
            // And capture the mouse.
            if (pt.x >= 0 && pt.x < _width && pt.y >= 0 && pt.y < _height)
            {
                mouse.OnMouseMove(pt.x, pt.y);
                if (!mouse.IsInWindow())
                {
                    SetCapture(_hwnd);
                    mouse.OnMouseEnter();
                }
            }
            else // Not in the window. But we still need to check if the mouse are clicked/holding down.
            {    // Therefore we still need to track the mouse position.
                if (wParam & (MK_LBUTTON | MK_RBUTTON))
                {
                    mouse.OnMouseMove(pt.x, pt.y);
                }
                else // And therefore if the mouse is not clicked/holding down,
                {    // we need to release the capture and trigger a leave event.
                    ReleaseCapture();
                    mouse.OnMouseLeave();
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            const POINTS pt {MAKEPOINTS(lParam)};
            mouse.OnLeftPressed(pt.x, pt.y);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            const POINTS pt {MAKEPOINTS(lParam)};
            mouse.OnRightPressed(pt.x, pt.y);
            break;
        }
        case WM_LBUTTONUP:
        {
            const POINTS pt {MAKEPOINTS(lParam)};
            mouse.OnLeftReleased(pt.x, pt.y);
            break;
        }
        case WM_RBUTTONUP:
        {
            const POINTS pt {MAKEPOINTS(lParam)};
            mouse.OnRightReleased(pt.x, pt.y);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            const POINTS pt {MAKEPOINTS(lParam)};
            mouse.OnWheelDelta(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
            break;
        }
        default:
            break;
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    // Window exception.
    Window::HrException::HrException(i32 line, const char* file, HRESULT hr) noexcept
        : Window::Exception {line, file}, _hr {hr} { }

    const char* Window::HrException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
            << std::dec << " (" << (u32) GetErrorCode() << ")" << std::endl
            << "[Description] " << GetErrorDescription() << std::endl
            << GetOriginString();

        _whatBuffer = oss.str();
        return _whatBuffer.c_str();
    }

    const char* Window::HrException::GetType() const noexcept
    {
        return "Drop Window Hr Exception";
    }

    std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
    {
        char*       pMsgBuf {nullptr};
        const DWORD nMsgLen {FormatMessageA(
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

    HRESULT Window::HrException::GetErrorCode() const noexcept
    {
        return _hr;
    }

    std::string Window::HrException::GetErrorDescription() const noexcept
    {
        return Exception::TranslateErrorCode(_hr);
    }

    const char* Window::NoGraphicsException::GetType() const noexcept
    {
        return "Drop Window No Graphics Exception";
    }
}