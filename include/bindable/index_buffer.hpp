#pragma once

#include "bindable.hpp"

namespace drop::bindable
{
    class IndexBuffer : public Bindable
    {
    public:
        IndexBuffer(graphics::Graphics& gfx, const std::vector<u16>& indices);
        void Bind(graphics::Graphics& gfx) noexcept override;
        u32  GetCount() const noexcept;

    protected:
        u32                                  _count;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBuffer;
    };

} // namespace drop::bindable