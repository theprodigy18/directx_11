#include "graphics/graphics.hpp"

#include "dxerr/dxerr.h"

#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

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
#define GFX_THROW_INFO_ONLY(call)                                                 \
    _dxgiInfoManager.Set();                                                       \
    (call);                                                                       \
    {                                                                             \
        auto v {_dxgiInfoManager.GetMessages()};                                  \
        if (!v.empty())                                                           \
            throw drop::graphics::Graphics::InfoException(__LINE__, __FILE__, v); \
    }
#else
#define GFX_EXCEPT(hr) drop::graphics::Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) drop::graphics::Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

namespace wrl = Microsoft::WRL;
namespace dx  = DirectX;

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

        wrl::ComPtr<ID3D11Resource> _pBackBuffer {nullptr};
        GFX_THROW_INFO(_pSwapChain->GetBuffer(
            0,
            __uuidof(ID3D11Resource),
            &_pBackBuffer));
        GFX_THROW_INFO(_pDevice->CreateRenderTargetView(
            _pBackBuffer.Get(),
            nullptr,
            &_pTargetView));

        // Create depth stencil state.
        D3D11_DEPTH_STENCIL_DESC dsd {};
        dsd.DepthEnable    = TRUE;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsd.DepthFunc      = D3D11_COMPARISON_LESS;

        wrl::ComPtr<ID3D11DepthStencilState> pDepthStencilState {nullptr};
        GFX_THROW_INFO(_pDevice->CreateDepthStencilState(
            &dsd,
            &pDepthStencilState));

        // Bind depth stencil to output.
        _pContext->OMSetDepthStencilState(
            pDepthStencilState.Get(),
            1);

        // Create depth stencil texture.
        wrl::ComPtr<ID3D11Texture2D> pDepthStencil {nullptr};
        D3D11_TEXTURE2D_DESC         descDepth {};
        descDepth.Width              = 1600;
        descDepth.Height             = 900;
        descDepth.MipLevels          = 1;
        descDepth.ArraySize          = 1;
        descDepth.Format             = DXGI_FORMAT_D32_FLOAT;
        descDepth.SampleDesc.Count   = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage              = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;

        GFX_THROW_INFO(_pDevice->CreateTexture2D(
            &descDepth,
            nullptr,
            &pDepthStencil));

        // Create view of the depth stencil texture.
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV {};
        descDSV.Format             = DXGI_FORMAT_D32_FLOAT;
        descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;

        GFX_THROW_INFO(_pDevice->CreateDepthStencilView(
            pDepthStencil.Get(),
            &descDSV,
            &_pDepthStencilView));

        // Bind the view to the pipeline.
        _pContext->OMSetRenderTargets(
            1,
            _pTargetView.GetAddressOf(),
            _pDepthStencilView.Get());
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

        _pContext->ClearRenderTargetView(_pTargetView.Get(), color);
        _pContext->ClearDepthStencilView(
            _pDepthStencilView.Get(),
            D3D11_CLEAR_DEPTH,
            1.0f, 0);
    }

    void Graphics::DrawTestTriangle(f32 angle, f32 x, f32 z)
    {
        HRESULT hr {};

        struct Vertex
        {
            struct
            {
                f32 x {0.0f};
                f32 y {0.0f};
                f32 z {0.0f};
            } pos;
        };

        const Vertex vertices[] {
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},
            {1.0f, 1.0f, -1.0f},
            {-1.0f, -1.0f, 1.0f},
            {1.0f, -1.0f, 1.0f},
            {-1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
        };

        // Create vertex buffer.
        wrl::ComPtr<ID3D11Buffer> pVertexBuffer {nullptr};

        D3D11_BUFFER_DESC vbd {};
        vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage               = D3D11_USAGE_DEFAULT;
        vbd.CPUAccessFlags      = 0;
        vbd.MiscFlags           = 0;
        vbd.ByteWidth           = sizeof(vertices);
        vbd.StructureByteStride = sizeof(Vertex);

        D3D11_SUBRESOURCE_DATA vsd {};
        vsd.pSysMem = vertices;

        GFX_THROW_INFO(_pDevice->CreateBuffer(
            &vbd,
            &vsd,
            &pVertexBuffer));

        // Bind vertex buffer to pipeline.
        const UINT stride {sizeof(Vertex)};
        const UINT offset {0};
        _pContext->IASetVertexBuffers(
            0, 1,
            pVertexBuffer.GetAddressOf(),
            &stride, &offset);

        // Create index buffer.
        const u16 indices[] {
            0, 2, 1, 2, 3, 1,
            1, 3, 5, 3, 7, 5,
            2, 6, 3, 3, 6, 7,
            4, 5, 7, 4, 7, 6,
            0, 4, 2, 2, 4, 6,
            0, 1, 4, 1, 5, 4};

        wrl::ComPtr<ID3D11Buffer> pIndexBuffer {nullptr};
        D3D11_BUFFER_DESC         ibd {};
        ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage               = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags      = 0;
        ibd.MiscFlags           = 0;
        ibd.ByteWidth           = sizeof(indices);
        ibd.StructureByteStride = sizeof(u16);

        D3D11_SUBRESOURCE_DATA isd {};
        isd.pSysMem = indices;

        GFX_THROW_INFO(_pDevice->CreateBuffer(
            &ibd,
            &isd,
            &pIndexBuffer));

        // Bind index buffer.
        _pContext->IASetIndexBuffer(
            pIndexBuffer.Get(),
            DXGI_FORMAT_R16_UINT,
            0);

        // Create constant buffer for transformation matrix.
        struct ConstantBuffer
        {
            dx::XMMATRIX transform;
        };

        const ConstantBuffer cb[] {
            {dx::XMMatrixTranspose(
                dx::XMMatrixRotationZ(angle) *
                dx::XMMatrixRotationX(angle) *
                dx::XMMatrixTranslation(x, 0.0f, z + 4.0f) *
                dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 10.0f))},
        };

        wrl::ComPtr<ID3D11Buffer> pConstantBuffer {nullptr};
        D3D11_BUFFER_DESC         cbd {};
        cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage               = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags           = 0;
        cbd.ByteWidth           = sizeof(cb);
        cbd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA csd {};
        csd.pSysMem = cb;

        GFX_THROW_INFO(_pDevice->CreateBuffer(
            &cbd,
            &csd,
            &pConstantBuffer));

        // Bind constant buffer.
        _pContext->VSSetConstantBuffers(
            0, 1,
            pConstantBuffer.GetAddressOf());

        struct ConstantBuffer2
        {
            struct
            {
                f32 r, g, b, a;
            } faceColor[6];
        };

        const ConstantBuffer2 cb2[] {
            {
                {{1.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f, 0.0f},
                 {0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f},
                 {1.0f, 1.0f, 0.0f},
                 {0.0f, 1.0f, 1.0f}},
            }};

        wrl::ComPtr<ID3D11Buffer> pConstantBuffer2 {nullptr};
        D3D11_BUFFER_DESC         cbd2 {};
        cbd2.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        cbd2.Usage               = D3D11_USAGE_DEFAULT;
        cbd2.CPUAccessFlags      = 0;
        cbd2.MiscFlags           = 0;
        cbd2.ByteWidth           = sizeof(cb2);
        cbd2.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA csd2 {};
        csd2.pSysMem = cb2;

        GFX_THROW_INFO(_pDevice->CreateBuffer(
            &cbd2,
            &csd2,
            &pConstantBuffer2));

        // Bind constant buffer.
        _pContext->PSSetConstantBuffers(
            0,
            1,
            pConstantBuffer2.GetAddressOf());

        // Create a pixel shader.
        wrl::ComPtr<ID3D11PixelShader>
                              pPixelShader {nullptr};
        wrl::ComPtr<ID3DBlob> pBlob {nullptr};

        GFX_THROW_INFO(D3DReadFileToBlob(L"basic_shader_ps.cso", &pBlob));
        GFX_THROW_INFO(_pDevice->CreatePixelShader(
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            nullptr,
            &pPixelShader));

        // Bind pixel shader.
        _pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

        // Create a vertex shader.
        wrl::ComPtr<ID3D11VertexShader> pVertexShader {nullptr};

        GFX_THROW_INFO(D3DReadFileToBlob(L"basic_shader_vs.cso", &pBlob));
        GFX_THROW_INFO(_pDevice->CreateVertexShader(
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            nullptr,
            &pVertexShader));

        // Bind vertex shader.
        _pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

        // Input layout(vertex) (2d position only).
        wrl::ComPtr<ID3D11InputLayout> pInputLayout {nullptr};
        const D3D11_INPUT_ELEMENT_DESC ied[] {
            {"Position",
             0,
             DXGI_FORMAT_R32G32B32_FLOAT,
             0,
             0,
             D3D11_INPUT_PER_VERTEX_DATA,
             0},
        };

        GFX_THROW_INFO(_pDevice->CreateInputLayout(
            ied,
            (UINT) std::size(ied),
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            &pInputLayout));

        // Bind vertex layout.
        _pContext->IASetInputLayout(pInputLayout.Get());

        // Set primitive topology.
        _pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Configure viewport.
        D3D11_VIEWPORT vp {};
        vp.Width    = 1600;
        vp.Height   = 900;
        vp.MinDepth = 0;
        vp.MaxDepth = 1;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        _pContext->RSSetViewports(1, &vp);

        // Draw.
        GFX_THROW_INFO_ONLY(_pContext->DrawIndexed((UINT) std::size(indices), 0, 0));
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

    Graphics::InfoException::InfoException(i32 line, const char* file, std::vector<std::string> infoMsgs) noexcept
        : Graphics::Exception {line, file}
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

    const char* Graphics::InfoException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << "\n[Error Info]\n"
            << GetErrorInfo() << std::endl
            << std::endl
            << GetOriginString();

        _whatBuffer = oss.str();
        return _whatBuffer.c_str();
    }

    const char* Graphics::InfoException::GetType() const noexcept
    {
        return "Drop Graphics Info Exception";
    }

    std::string Graphics::InfoException::GetErrorInfo() const noexcept
    {
        return _info;
    }

    const char* Graphics::DeviceRemovedException::GetType() const noexcept
    {
        return "Drop Graphics Device Removed Exception";
    }
} // namespace drop::graphics