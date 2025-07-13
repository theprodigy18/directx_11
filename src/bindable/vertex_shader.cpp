#include "bindable/vertex_shader.hpp"

#include "utils/graphics_throw_macros.hpp"

namespace drop::bindable
{
    VertexShader::VertexShader(graphics::Graphics& gfx, const std::wstring& path)
    {
        INFOMAN(gfx);

        GFX_THROW_INFO(D3DReadFileToBlob(
            path.c_str(),
            &_pBlob));

        GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
            _pBlob->GetBufferPointer(),
            _pBlob->GetBufferSize(),
            nullptr,
            &_pVertexShader));
    }

    void VertexShader::Bind(graphics::Graphics& gfx) noexcept
    {
        GetContext(gfx)->VSSetShader(
            _pVertexShader.Get(),
            nullptr,
            0);
    }

    ID3DBlob* VertexShader::GetBlob() const noexcept
    {
        return _pBlob.Get();
    }
}