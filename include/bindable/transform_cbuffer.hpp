#pragma once

#include "bindable.hpp"
#include "bindable/constant_buffers.hpp"
#include "drawable/drawable.hpp"

#include <DirectXMath.h>

namespace drop::bindable
{
    class TransformCbuffer : public Bindable
    {
    public:
        TransformCbuffer(graphics::Graphics& gfx, const drawable::Drawable& parent);
        void Bind(graphics::Graphics& gfx) noexcept override;

    private:
        VertexConstantBuffer<DirectX::XMMATRIX> _vCbuffer;
        const drawable::Drawable&               _parent;
    };
} // namespace drop::bindable