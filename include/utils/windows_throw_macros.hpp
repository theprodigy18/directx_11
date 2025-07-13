#pragma once

#define HWND_EXCEPT(hr) drop::platform::Window::HrException(__LINE__, __FILE__, hr)
#define HWND_LAST_EXCEPT() drop::platform::Window::HrException(__LINE__, __FILE__, GetLastError())
#define HWND_NOGFX_EXCEPT() drop::platform::Window::NoGraphicsException(__LINE__, __FILE__);
