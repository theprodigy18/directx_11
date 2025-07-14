#pragma once

#include "graphics/graphics.hpp"

namespace drop::bindable
{

    class Bindable
    {
    public:
        virtual void Bind(graphics::Graphics& gfx) noexcept = 0;
        virtual ~Bindable()                                 = default;

    protected:
        static ID3D11DeviceContext*       GetContext(graphics::Graphics& gfx) noexcept;
        static ID3D11Device*              GetDevice(graphics::Graphics& gfx) noexcept;
        static dxhelper::DxgiInfoManager& GetInfoManager(graphics::Graphics& gfx) noexcept(!IS_DEBUG);
    };

} // namespace drop::graphics