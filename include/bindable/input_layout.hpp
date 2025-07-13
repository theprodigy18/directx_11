#pragma once

#include "bindable.hpp"

namespace drop::bindable
{
    class InputLayout : public Bindable
    {
    public:
        InputLayout(graphics::Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                    ID3DBlob* pVertexShaderBlob);
        void Bind(graphics::Graphics& gfx) noexcept override;

    protected:
        Microsoft::WRL::ComPtr<ID3D11InputLayout> _pInputLayout;
    };
} // namespace drop::bindable