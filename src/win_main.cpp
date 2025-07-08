#include "core/engine.hpp"

using namespace drop;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SetProcessDPIAware();

    D_TRACE("Drop Engine started.");

    try
    {
        return core::Engine {}.Run();
    }
    catch (platform::Window::Exception& e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (std::exception& e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, "No additional information available.", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return -1;
}