#include "bindable/index_buffer.hpp"

#include "utils/graphics_throw_macros.hpp"

namespace drop::bindable
{
    IndexBuffer::IndexBuffer(graphics::Graphics& gfx, const std::vector<u16>& indices)
        : _count((u32) indices.size())
    {
        INFOMAN(gfx);

        D3D11_BUFFER_DESC ibd {};
        ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage               = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags      = 0u;
        ibd.MiscFlags           = 0u;
        ibd.ByteWidth           = UINT(_count * sizeof(u16));
        ibd.StructureByteStride = sizeof(u16);

        D3D11_SUBRESOURCE_DATA isd {};
        isd.pSysMem = indices.data();

        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(
            &ibd,
            &isd,
            &_pIndexBuffer));
    }

    void IndexBuffer::Bind(graphics::Graphics& gfx) noexcept
    {
        GetContext(gfx)->IASetIndexBuffer(
            _pIndexBuffer.Get(),
            DXGI_FORMAT_R16_UINT,
            0);
    }

    u32 IndexBuffer::GetCount() const noexcept
    {
        return _count;
    }

} // namespace drop::bindable