#include "drawable/drawable.hpp"
#include "bindable/index_buffer.hpp"

namespace drop::drawable
{
    void Drawable::Draw(graphics::Graphics& gfx) const noexcept(!_DEBUG)
    {
        for (auto& b : _binds)
        {
            b->Bind(gfx);
        }
        gfx.DrawIndexed(_pIndexBuffer->GetCount());
    }

    void Drawable::AddBind(std::unique_ptr<bindable::Bindable> bind) noexcept(!_DEBUG)
    {
        D_ASSERT(typeid(*bind) != typeid(bindable::IndexBuffer), "IndexBuffer not allowed.");
        _binds.push_back(std::move(bind));
    }

    void Drawable::AddIndexBuffer(std::unique_ptr<bindable::IndexBuffer> indexBuffer) noexcept
    {
        D_ASSERT(!_pIndexBuffer, "IndexBuffer already set.");
        _pIndexBuffer = indexBuffer.get();
        _binds.push_back(std::move(indexBuffer));
    }
} // namespace drop::drawable