#include "bindable/pixel_shader.hpp"

#include "utils/graphics_throw_macros.hpp"

namespace drop::bindable
{
    PixelShader::PixelShader(graphics::Graphics& gfx, const std::wstring& path)
    {
        INFOMAN(gfx);

        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
        GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBlob));
        GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(
            pBlob->GetBufferPointer(),
            pBlob->GetBufferSize(),
            nullptr,
            &_pPixelShader));
    }

    void PixelShader::Bind(graphics::Graphics& gfx) noexcept
    {
        GetContext(gfx)->PSSetShader(
            _pPixelShader.Get(),
            nullptr,
            0);
    }
} // namespace drop::bindable