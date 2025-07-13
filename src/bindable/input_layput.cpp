#include "bindable/input_layout.hpp"

#include "utils/graphics_throw_macros.hpp"

namespace drop::bindable
{

    InputLayout::InputLayout(graphics::Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                             ID3DBlob* pVertexShaderBlob)
    {
        INFOMAN(gfx);

        GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
            layout.data(),
            (UINT) layout.size(),
            pVertexShaderBlob->GetBufferPointer(),
            pVertexShaderBlob->GetBufferSize(),
            &_pInputLayout));
    }

    void InputLayout::Bind(graphics::Graphics& gfx) noexcept
    {
        GetContext(gfx)->IASetInputLayout(_pInputLayout.Get());
    }
} // namespace drop::bindable