#include "bindable/transform_cbuffer.hpp"

namespace drop::bindable
{
    TransformCbuffer::TransformCbuffer(graphics::Graphics& gfx, const drawable::Drawable& parent)
        : _vCbuffer {gfx}, _parent {parent} { }

    void TransformCbuffer::Bind(graphics::Graphics& gfx) noexcept
    {
        _vCbuffer.Update(
            gfx,
            DirectX::XMMatrixTranspose(
                _parent.GetTransformXM() * gfx.GetProjection()));

        _vCbuffer.Bind(gfx);
    }
} // namespace drop::bindable