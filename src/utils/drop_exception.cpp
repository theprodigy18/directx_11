#include "utils/drop_exception.hpp"

#include <sstream>

namespace drop::utils
{
    DropException::DropException(i32 line, const char* file) noexcept
        : _line(line), _file(file) { }

    const char* DropException::what() const noexcept
    {
        std::ostringstream oss;
        oss << GetType() << std::endl
            << GetOriginString();
        _whatBuffer = oss.str();
        return _whatBuffer.c_str();
    }

    const char* DropException::GetType() const noexcept
    {
        return "Drop Exception";
    }

    i32 DropException::GetLine() const noexcept
    {
        return _line;
    }

    const std::string& DropException::GetFile() const noexcept
    {
        return _file;
    }

    std::string DropException::GetOriginString() const noexcept
    {
        std::ostringstream oss;
        oss << "[File] " << _file << std::endl
            << "[Line] " << _line;
        return oss.str();
    }
} // namespace drop::utils