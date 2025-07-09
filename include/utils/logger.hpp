#pragma once

#include <cstdio>
#include <cstdarg>
#include <windows.h>

#ifdef _DEBUG
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK()
#endif // _DEBUG

#ifdef _DEBUG
// --- Text Color ---
enum TextColor
{
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_BRIGHT_BLACK,
    TEXT_COLOR_BRIGHT_RED,
    TEXT_COLOR_BRIGHT_GREEN,
    TEXT_COLOR_BRIGHT_YELLOW,
    TEXT_COLOR_BRIGHT_BLUE,
    TEXT_COLOR_BRIGHT_MAGENTA,
    TEXT_COLOR_BRIGHT_CYAN,
    TEXT_COLOR_BRIGHT_WHITE,
    TEXT_COLOR_COUNT
};

namespace
{
    // --- Internal Logging ---
    inline void _LogVA(const char* prefix, const char* file, int line, TextColor, const char* fmt, va_list args)
    {
        char msgBuffer[4096];
        vsnprintf(msgBuffer, sizeof(msgBuffer), fmt, args);

        char finalBuffer[4608];
        snprintf(finalBuffer, sizeof(finalBuffer), "%s [%s:%d] %s\n", prefix, file, line, msgBuffer);

        OutputDebugStringA(finalBuffer);
    }

    inline void _Log(const char* prefix, const char* file, int line, TextColor color, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        _LogVA(prefix, file, line, color, fmt, args);
        va_end(args);
    }
} // namespace anonymous

#define D_TRACE(...) _Log("TRACE: ", __FILE__, __LINE__, TEXT_COLOR_GREEN, __VA_ARGS__) // Log Trace.
#define D_WARN(...) _Log("WARN:  ", __FILE__, __LINE__, TEXT_COLOR_YELLOW, __VA_ARGS__) // Log Warning.
#define D_ERROR(...) _Log("ERROR: ", __FILE__, __LINE__, TEXT_COLOR_RED, __VA_ARGS__)   // Log Error.

#define D_ASSERT(cond, ...)                      \
    do                                           \
    {                                            \
        if (!(cond))                             \
        {                                        \
            D_ERROR(__VA_ARGS__);                \
            D_ERROR("Assertion HIT: %s", #cond); \
            DEBUG_BREAK();                       \
        }                                        \
    } while (0)

#else
#define D_TRACE(...)
#define D_WARN(...)
#define D_ERROR(...)
#define D_ASSERT(cond, ...)
#endif // _DEBUG
