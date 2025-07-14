#pragma once

#include "graphics/graphics.hpp"
#include "bindable/bindable.hpp"

#include <memory>
#include <DirectXMath.h>

namespace drop::bindable
{
    class IndexBuffer;
}

namespace drop::drawable
{
    class Drawable
    {
    public:
        Drawable()                                                = default;
        Drawable(const Drawable&)                                 = delete; // Copy Constructor.
        virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
        void                      Draw(graphics::Graphics& gfx) const noexcept(!IS_DEBUG);
        virtual void              Update(f32 dt) noexcept = 0;
        void                      AddBind(std::unique_ptr<bindable::Bindable> bind) noexcept(!IS_DEBUG);
        void                      AddIndexBuffer(std::unique_ptr<bindable::IndexBuffer> indexBuffer) noexcept;
        virtual ~Drawable() = default;

    private:
        const bindable::IndexBuffer*                     _pIndexBuffer {nullptr};
        std::vector<std::unique_ptr<bindable::Bindable>> _binds;
    };
} // namespace drop::drawable