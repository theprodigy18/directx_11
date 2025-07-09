#include "graphics/graphics.hpp"

#include "dxerr/dxerr.h"
#include <sstream>

// Graphics exception checking/throwing macros (some with dxgi infos).
#define GFX_EXCEPT_NOINFO(hr) drop::graphics::Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) \
    if (FAILED(hr = (hrcall))) throw drop::graphics::Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) drop::graphics::Graphics::HrException(__LINE__, __FILE__, (hr), _dxgiInfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) \
    _dxgiInfoManager.Set();    \
    if (FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) \
    drop::graphics::Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), _dxgiInfoManager.GetMessages())
#else
#define GFX_EXCEPT(hr) drop::graphics::Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) drop::graphics::Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#endif

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

        i32 swapCreateFlags {0};
#ifdef _DEBUG
        swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

        HRESULT hr {0};

        GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            swapCreateFlags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &scd,
            &_pSwapChain,
            &_pDevice,
            nullptr,
            &_pContext));

        ID3D11Resource* _pBackBuffer {nullptr};
        GFX_THROW_INFO(_pSwapChain->GetBuffer(
            0,
            __uuidof(ID3D11Resource),
            reinterpret_cast<void**>(&_pBackBuffer)));
        GFX_THROW_INFO(_pDevice->CreateRenderTargetView(
            _pBackBuffer,
            nullptr,
            &_pTargetView));

        _pBackBuffer->Release();
    }

    Graphics::~Graphics()
    {
        if (_pTargetView)
        {
            _pTargetView->Release();
        }
        if (_pContext)
        {
            _pContext->Release();
        }

        if (_pSwapChain)
        {
            _pSwapChain->Release();
        }

        if (_pDevice)
        {
            _pDevice->Release();
        }
    }

    void Graphics::BeginFrame()
    {
    }

    void Graphics::EndFrame()
    {
        HRESULT hr {};
#ifdef _DEBUG
        _dxgiInfoManager.Set();
#endif // _DEBUG
        if (FAILED(hr = _pSwapChain->Present(1, 0)))
        {
            if (hr == DXGI_ERROR_DEVICE_REMOVED)
            {
                throw GFX_DEVICE_REMOVED_EXCEPT(_pDevice->GetDeviceRemovedReason());
            }
            else
            {
                throw GFX_EXCEPT(hr);
            }
        }
    }

    void Graphics::ClearBuffer(f32 r, f32 g, f32 b) noexcept
    {
        const f32 color[] {
            r,
            g,
            b,
            1.0f};

        _pContext->ClearRenderTargetView(_pTargetView, color);
    }

    // Exception.
    Graphics::HrException::HrException(i32 line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
        : Graphics::Exception {line, file}, _hr {hr}
    {
        // Join all info messages with newlines into single string.
        for (const auto& m : infoMsgs)
        {
            _info += m;
            _info.push_back('\n');
        }
        // Remove final newline if exists.
        if (!_info.empty())
        {
            _info.pop_back();
        }
    }

    const char* Graphics::HrException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
            << std::dec << " (" << (u32) GetErrorCode() << ")" << std::endl
            << "[Error String] " << GetErrorString() << std::endl
            << "[Description] " << GetErrorDescription() << std::endl;
        if (!_info.empty())
        {
            oss << "\n[Error Info]\n"
                << GetErrorInfo() << std::endl
                << std::endl;
        }
        oss << GetOriginString();

        _whatBuffer = oss.str();
        return _whatBuffer.c_str();
    }

    const char* Graphics::HrException::GetType() const noexcept
    {
        return "Drop Graphics Hr Exception";
    }

    HRESULT Graphics::HrException::GetErrorCode() const noexcept
    {
        return _hr;
    }

    std::string Graphics::HrException::GetErrorString() const noexcept
    {
        return DXGetErrorStringA(_hr);
    }

    std::string Graphics::HrException::GetErrorDescription() const noexcept
    {
        char buf[512];
        DXGetErrorDescriptionA(_hr, buf, sizeof(buf));
        return buf;
    }

    std::string Graphics::HrException::GetErrorInfo() const noexcept
    {
        return _info;
    }

    const char* Graphics::DeviceRemovedException::GetType() const noexcept
    {
        return "Drop Graphics Device Removed Exception";
    }
} // namespace drop::graphics