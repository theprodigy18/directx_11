#pragma once

#include "bindable.hpp"

namespace drop::bindable
{
    class PixelShader : public Bindable
    {
    public:
        PixelShader(graphics::Graphics& gfx, const std::wstring& path);
        void Bind(graphics::Graphics& gfx) noexcept override;

    protected:
        Microsoft::WRL::ComPtr<ID3D11PixelShader> _pPixelShader;
    };
} // namespace drop::bindable