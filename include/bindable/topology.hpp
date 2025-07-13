#pragma once

#include "bindable.hpp"

namespace drop::bindable
{

    class Topology : public Bindable
    {
    public:
        Topology(graphics::Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
        void Bind(graphics::Graphics& gfx) noexcept override;

    protected:
        D3D11_PRIMITIVE_TOPOLOGY _type;
    };
} // namespace drop::bindable