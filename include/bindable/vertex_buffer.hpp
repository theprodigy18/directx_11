#pragma once

#include "bindable.hpp"

#include "utils/graphics_throw_macros.hpp"

namespace drop::bindable
{
    class VertexBuffer : public Bindable
    {
    public:
        template <class V>
        VertexBuffer(graphics::Graphics& gfx, const std::vector<V>& vertices)
            : _stride {(u32) sizeof(V)}
        {
            INFOMAN(gfx);

            D3D11_BUFFER_DESC vbd {};
            vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
            vbd.Usage               = D3D11_USAGE_DEFAULT;
            vbd.CPUAccessFlags      = 0u;
            vbd.MiscFlags           = 0u;
            vbd.ByteWidth           = UINT(sizeof(V) * vertices.size());
            vbd.StructureByteStride = sizeof(V);

            D3D11_SUBRESOURCE_DATA vsd {};
            vsd.pSysMem = vertices.data();

            GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(
                &vbd,
                &vsd,
                &_pVertexBuffer));
        }

        void Bind(graphics::Graphics& gfx) noexcept override;

    protected:
        u32                                  _stride;
        Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBuffer;
    };
} // namespace drop::bindable