#pragma once

#include <Windows.h>
#include <d3d11.h>

namespace drop::graphics
{
    class Graphics
    {
    public:
        Graphics(HWND handle);
        Graphics(const Graphics&)            = delete; // Copy Constructor.
        Graphics& operator=(const Graphics&) = delete; // Copy Assignment Operator.
        ~Graphics();

        void BeginFrame();
        void EndFrame();
        void ClearBuffer(f32 r, f32 g, f32 b) noexcept;

    private:
        ID3D11Device*           _device {nullptr};
        IDXGISwapChain*         _swapChain {nullptr};
        ID3D11DeviceContext*    _context {nullptr};
        ID3D11RenderTargetView* _targetView {nullptr};
    };
} // namespace drop::graphics