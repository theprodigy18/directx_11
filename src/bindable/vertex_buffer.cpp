#include "bindable/vertex_buffer.hpp"

namespace drop::bindable
{
    void VertexBuffer::Bind(graphics::Graphics& gfx) noexcept
    {
        const UINT offset {0};
        GetContext(gfx)->IASetVertexBuffers(
            0,
            1,
            _pVertexBuffer.GetAddressOf(),
            &_stride,
            &offset);
    }
} // namespace drop::bindable