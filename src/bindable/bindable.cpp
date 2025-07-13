#include "bindable/bindable.hpp"

namespace drop::bindable
{
    ID3D11DeviceContext* Bindable::GetContext(graphics::Graphics& gfx) noexcept
    {
        return gfx._pContext.Get();
    }

    ID3D11Device* Bindable::GetDevice(graphics::Graphics& gfx) noexcept
    {
        return gfx._pDevice.Get();
    }

    dxhelper::DxgiInfoManager& Bindable::GetInfoManager(graphics::Graphics& gfx) noexcept(!_DEBUG)
    {
#ifdef _DEBUG
        return gfx._dxgiInfoManager;
#else
        throw std::logic_error("YouFuckedUp! (tried to access gfx.infoManager in Release config)");
#endif
    }
} // namespace drop::bindable