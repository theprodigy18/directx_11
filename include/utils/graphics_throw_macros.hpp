#pragma once

// Graphics exception checking/throwing macros (some with dxgi infos).
#define GFX_EXCEPT_NOINFO(hr) drop::graphics::Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) \
    if (FAILED(hr = (hrcall))) throw drop::graphics::Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) drop::graphics::Graphics::HrException(__LINE__, __FILE__, (hr), _dxgiInfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) \
    _dxgiInfoManager.Set();    \
    if (FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) \
    drop::graphics::Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), _dxgiInfoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call)                                                 \
    _dxgiInfoManager.Set();                                                       \
    (call);                                                                       \
    {                                                                             \
        auto v {_dxgiInfoManager.GetMessages()};                                  \
        if (!v.empty())                                                           \
            throw drop::graphics::Graphics::InfoException(__LINE__, __FILE__, v); \
    }
#else
#define GFX_EXCEPT(hr) drop::graphics::Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) drop::graphics::Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define INFOMAN(gfx)                     \
    HRESULT                          hr; \
    drop::dxhelper::DxgiInfoManager& _dxgiInfoManager {drop::bindable::Bindable::GetInfoManager((gfx))};
#endif
