#include "bindable/topology.hpp"

namespace drop::bindable
{
    Topology::Topology(graphics::Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
        : _type(type) { }

    void Topology::Bind(graphics::Graphics& gfx) noexcept
    {
        GetContext(gfx)->IASetPrimitiveTopology(_type);
    }
} // namespace drop::bindable