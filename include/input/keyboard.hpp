#pragma once

#include <queue>
#include <bitset>

namespace drop::platform
{
    class Window;
} // namespace drop::platform

namespace drop::input
{

    class Keyboard
    {
        friend class platform::Window;

    public:
        class Event
        {
        public:
            enum class Type
            {
                PRESS,
                RELEASE,
                INVALID
            };

        private:
            Type _type {Type::INVALID};
            u8   _keyCode {0};

        public:
            Event() noexcept
                : _type {Type::INVALID}, _keyCode {0} { }
            Event(Type type, u8 keyCode) noexcept
                : _type {type}, _keyCode {keyCode} { }

            bool IsPressed() const noexcept
            {
                return _type == Type::PRESS;
            }

            bool IsReleased() const noexcept
            {
                return _type == Type::RELEASE;
            }

            u8 GetKeyCode() const noexcept
            {
                return _keyCode;
            }
        };

    public:
        Keyboard()                           = default;
        Keyboard(const Keyboard&)            = delete;
        Keyboard& operator=(const Keyboard&) = delete;

        // Key event.
        bool  IsKeyPressed(u8 keyCode) const noexcept;
        Event ReadKey() noexcept;
        bool  IsKeyEmpty() const noexcept;
        void  FlushKey() noexcept;

        // Char event.
        u8   ReadChar() noexcept;
        bool IsCharEmpty() const noexcept;
        void FlushChar() noexcept;

        void Flush() noexcept; // Flush both key and char events.

        // Auto repeat control.
        void EnableAutoRepeat() noexcept;
        void DisableAutoRepeat() noexcept;
        bool IsAutoRepeatEnabled() const noexcept;

    private:
        void OnKeyPressed(u8 keyCode) noexcept;
        void OnKeyReleased(u8 keyCode) noexcept;
        void OnChar(u8 charCode) noexcept;
        void ClearState() noexcept;

        template <typename T>
        static void TrimBuffer(std::queue<T>& buffer) noexcept;

        static constexpr u32  s_N_KEYS {256};
        static constexpr u32  s_BUFFER_SIZE {16};
        bool                  _autoRepeatEnabled {false};
        std::bitset<s_N_KEYS> _keyStates;
        std::queue<Event>     _keyBuffer;
        std::queue<u8>        _charBuffer;
    };
} // namespace drop::input