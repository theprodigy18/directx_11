#pragma once

#include "bindable.hpp"

#include "utils/graphics_throw_macros.hpp"

namespace drop::bindable
{
    template <typename C>
    class ConstantBuffer : public Bindable
    {
    public:
        void Update(graphics::Graphics& gfx, const C& consts)
        {
            INFOMAN(gfx);
            D3D11_MAPPED_SUBRESOURCE msr {};
            GFX_THROW_INFO(GetContext(gfx)->Map(
                _pConstantBuffer.Get(),
                0,
                D3D11_MAP_WRITE_DISCARD,
                0,
                &msr));

            memcpy(msr.pData, &consts, sizeof(consts));
            GetContext(gfx)->Unmap(
                _pConstantBuffer.Get(),
                0);
        }

        ConstantBuffer(graphics::Graphics& gfx, const C& consts)
        {
            INFOMAN(gfx);

            D3D11_BUFFER_DESC cbd {};
            cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
            cbd.Usage               = D3D11_USAGE_DYNAMIC;
            cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
            cbd.MiscFlags           = 0u;
            cbd.ByteWidth           = sizeof(consts);
            cbd.StructureByteStride = 0u;

            D3D11_SUBRESOURCE_DATA csd {};
            csd.pSysMem = &consts;
            GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(
                &cbd,
                &csd,
                &_pConstantBuffer));
        }
        ConstantBuffer(graphics::Graphics& gfx)
        {
            INFOMAN(gfx);

            D3D11_BUFFER_DESC cbd {};
            cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
            cbd.Usage               = D3D11_USAGE_DYNAMIC;
            cbd.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
            cbd.MiscFlags           = 0u;
            cbd.ByteWidth           = sizeof(C);
            cbd.StructureByteStride = 0u;
            GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(
                &cbd,
                nullptr,
                &_pConstantBuffer));
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;
    };

    template <typename C>
    class VertexConstantBuffer : public ConstantBuffer<C>
    {
        using ConstantBuffer<C>::_pConstantBuffer;
        using Bindable::GetContext;

    public:
        using ConstantBuffer<C>::ConstantBuffer;
        void Bind(graphics::Graphics& gfx) noexcept override
        {
            GetContext(gfx)->VSSetConstantBuffers(
                0u,
                1u,
                _pConstantBuffer.GetAddressOf());
        }
    };

    template <typename C>
    class PixelConstantBuffer : public ConstantBuffer<C>
    {
        using ConstantBuffer<C>::_pConstantBuffer;
        using Bindable::GetContext;

    public:
        using ConstantBuffer<C>::ConstantBuffer;
        void Bind(graphics::Graphics& gfx) noexcept override
        {
            GetContext(gfx)->PSSetConstantBuffers(
                0u,
                1u,
                _pConstantBuffer.GetAddressOf());
        }
    };
} // namespace drop::bindable