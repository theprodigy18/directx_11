#pragma once

#include "utils/drop_exception.hpp"
#include "dx_helper/dxgi_info_manager.hpp"

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace drop::bindable
{
    class Bindable;
} // namespace drop::bindable

namespace drop::graphics
{
    class Graphics
    {
        friend class bindable::Bindable;

    public:
        class Exception : public utils::DropException
        {
            using utils::DropException::DropException;
        };
        class HrException : public Exception
        {
        public:
            HrException(i32 line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
            const char* what() const noexcept override;
            const char* GetType() const noexcept override;
            HRESULT     GetErrorCode() const noexcept;
            std::string GetErrorString() const noexcept;
            std::string GetErrorDescription() const noexcept;
            std::string GetErrorInfo() const noexcept;

        private:
            HRESULT     _hr;
            std::string _info;
        };
        class InfoException : public Exception
        {
        public:
            InfoException(i32 line, const char* file, std::vector<std::string> infoMsgs = {}) noexcept;
            const char* what() const noexcept override;
            const char* GetType() const noexcept override;
            std::string GetErrorInfo() const noexcept;

        private:
            std::string _info;
        };
        class DeviceRemovedException : public HrException
        {
            using HrException::HrException;

        public:
            const char* GetType() const noexcept override;

        private:
            std::string _reason;
        };

    public:
        Graphics(HWND handle);
        Graphics(const Graphics&)            = delete; // Copy Constructor.
        Graphics& operator=(const Graphics&) = delete; // Copy Assignment Operator.
        ~Graphics()                          = default;

        void              BeginFrame();
        void              EndFrame();
        void              ClearBuffer(f32 r, f32 g, f32 b) noexcept;
        void              DrawIndexed(u32 count) noexcept(!IS_DEBUG);
        void              DrawTestTriangle(f32 angle, f32 x, f32 z);
        void              SetProjection(DirectX::FXMMATRIX proj) noexcept;
        DirectX::XMMATRIX GetProjection() const noexcept;

    private:
        DirectX::XMMATRIX projection;
#ifdef _DEBUG
        dxhelper::DxgiInfoManager _dxgiInfoManager;
#endif // _DEBUG
        Microsoft::WRL::ComPtr<ID3D11Device>           _pDevice {nullptr};
        Microsoft::WRL::ComPtr<IDXGISwapChain>         _pSwapChain {nullptr};
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>    _pContext {nullptr};
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pTargetView {nullptr};
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _pDepthStencilView {nullptr};
    };
} // namespace drop::graphics