#pragma once

#include <cstdio>
#include <cstdarg>

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
    inline void _LogVA(const char* prefix, const char* file, int line, TextColor color, const char* fmt, va_list args)
    {
        static const char* colorCodes[TEXT_COLOR_COUNT] = {
            "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m",
            "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m",
            "\x1b[90m", "\x1b[91m", "\x1b[92m", "\x1b[93m",
            "\x1b[94m", "\x1b[95m", "\x1b[96m", "\x1b[97m"};

        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        printf("%s%s [%s:%d] %s\x1b[0m\n", colorCodes[color], prefix, file, line, buffer);
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
#define D_LEAK(...)
#define D_ASSERT(cond, ...)
#endif // _DEBUG
