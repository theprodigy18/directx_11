#pragma once

#include "bindable.hpp"

namespace drop::bindable
{
    class VertexShader : public Bindable
    {
    public:
        VertexShader(graphics::Graphics& gfx, const std::wstring& path);
        void      Bind(graphics::Graphics& gfx) noexcept override;
        ID3DBlob* GetBlob() const noexcept;

    protected:
        Microsoft::WRL::ComPtr<ID3D10Blob>         _pBlob;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> _pVertexShader;
    };
} // namespace drop::bindable