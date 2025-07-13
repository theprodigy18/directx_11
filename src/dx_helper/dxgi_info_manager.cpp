#include "dx_helper/dxgi_info_manager.hpp"
#include "graphics/graphics.hpp" // IWYU pragma: keep
#include "platform/window.hpp"   // IWYU pragma: keep

#include <memory>

#include "utils/graphics_throw_macros.hpp"
#include "utils/windows_throw_macros.hpp"

namespace drop::dxhelper
{
    DxgiInfoManager::DxgiInfoManager()
    {
        // Define function signature for DXGIGetDebugInterface.
        typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void**);

        // Load dll that contains the function DXGIGetDebugInterface.
        const auto hModDxgiDebug {LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)};
        if (!hModDxgiDebug)
        {
            HWND_LAST_EXCEPT();
        }

        // Get the address/pointer to the function DXGIGetDebugInterface.
        const auto DxgiGetDebugInterface {reinterpret_cast<DXGIGetDebugInterface>(
            reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")))};
        if (!DxgiGetDebugInterface)
        {
            HWND_LAST_EXCEPT();
        }

        HRESULT hr {0};
        GFX_THROW_NOINFO(DxgiGetDebugInterface(
            __uuidof(IDXGIInfoQueue),
            &_pDxgiInfoQueue));
    }

    void DxgiInfoManager::Set() noexcept
    {
        _next = _pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
    }

    std::vector<std::string> DxgiInfoManager::GetMessages() const
    {
        std::vector<std::string> messages;
        const u64                end {_pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL)};
        for (u64 i {_next}; i < end; i++)
        {
            HRESULT hr {0};
            SIZE_T  messageLength {0};
            GFX_THROW_NOINFO(_pDxgiInfoQueue->GetMessageA(
                DXGI_DEBUG_ALL,
                i,
                nullptr,
                &messageLength));
            auto bytes {std::make_unique<byte[]>(messageLength)};
            auto pMessage {reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get())};
            GFX_THROW_NOINFO(_pDxgiInfoQueue->GetMessageA(
                DXGI_DEBUG_ALL,
                i,
                pMessage,
                &messageLength));
            messages.emplace_back(pMessage->pDescription);
        }

        return messages;
    }
} // namespace drop::dxhelper