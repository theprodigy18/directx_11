#pragma once

#include <exception>
#include <string>

namespace drop::utils
{

    class DropException : public std::exception
    {
    public:
        DropException(i32 line, const char* file) noexcept;
        const char*         what() const noexcept override;
        virtual const char* GetType() const noexcept;
        i32                 GetLine() const noexcept;
        const std::string&  GetFile() const noexcept;
        std::string         GetOriginString() const noexcept;

    private:
        i32         _line;
        std::string _file;

    protected:
        mutable std::string _whatBuffer;
    };

} // namespace drop::utils