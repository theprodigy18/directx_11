
#include "platform/window.hpp"

using namespace drop;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SetProcessDPIAware();

    D_TRACE("Drop Engine started.");
    i32 exitCode {0};

    try
    {
        platform::Window wnd {1600, 900, "Drop Engine"};

        bool running {true};
        while (running)
        {
            MSG msg {};
            while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);

                if (wnd.keyboard.IsKeyPressed(VK_MENU))
                {
                    MessageBox(nullptr, "Space key was pressed.", "Drop Engine", MB_OK | MB_ICONINFORMATION);
                }

                if (msg.message == WM_QUIT)
                {
                    running  = false;
                    exitCode = msg.wParam;
                    break;
                }
            }
        }
    }
    catch (platform::Window::Exception& e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
        exitCode = -1;
    }
    catch (std::exception& e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
        exitCode = -1;
    }
    catch (...)
    {
        MessageBox(nullptr, "No additional information available.", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
        exitCode = -1;
    }

    return exitCode;
}