#include "graphics/graphics.hpp"

namespace drop::graphics
{

    Graphics::Graphics(HWND handle)
    {
        DXGI_SWAP_CHAIN_DESC scd {};
        scd.BufferDesc.Width                   = 0;
        scd.BufferDesc.Height                  = 0;
        scd.BufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;
        scd.BufferDesc.RefreshRate.Numerator   = 0;
        scd.BufferDesc.RefreshRate.Denominator = 0;
        scd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
        scd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        scd.SampleDesc.Count                   = 1;
        scd.SampleDesc.Quality                 = 0;
        scd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount                        = 1;
        scd.OutputWindow                       = handle;
        scd.Windowed                           = TRUE;
        scd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
        scd.Flags                              = 0;

        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &scd,
            &_swapChain,
            &_device,
            nullptr,
            &_context);

        ID3D11Resource* _backBuffer {nullptr};
        _swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&_backBuffer));
        _device->CreateRenderTargetView(
            _backBuffer,
            nullptr,
            &_targetView);

        _backBuffer->Release();
    }

    Graphics::~Graphics()
    {
        if (_targetView)
        {
            _targetView->Release();
        }
        if (_context)
        {
            _context->Release();
        }

        if (_swapChain)
        {
            _swapChain->Release();
        }

        if (_device)
        {
            _device->Release();
        }
    }

    void Graphics::BeginFrame()
    {
    }

    void Graphics::EndFrame()
    {
        _swapChain->Present(1, 0);
    }

    void Graphics::ClearBuffer(f32 r, f32 g, f32 b) noexcept
    {
        const f32 color[] {
            r,
            g,
            b,
            1.0f};

        _context->ClearRenderTargetView(_targetView, color);
    }
} // namespace drop::graphics