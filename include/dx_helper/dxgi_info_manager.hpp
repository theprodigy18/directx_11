#pragma once

#include <dxgidebug.h>
#include <vector>
#include <string>

namespace drop::dxhelper
{
    class DxgiInfoManager
    {
    public:
        DxgiInfoManager();
        DxgiInfoManager(const DxgiInfoManager&)            = delete; // Copy Constructor.
        DxgiInfoManager& operator=(const DxgiInfoManager&) = delete; // Copy Assignment Operator.
        ~DxgiInfoManager();

        void                     Set() noexcept;
        std::vector<std::string> GetMessages() const;

    private:
        u64                    _next {0};
        struct IDXGIInfoQueue* _pDxgiInfoQueue {nullptr};
    };
} // namespace drop::dxhelper