#include "input/keyboard.hpp"

namespace drop::input
{

    bool Keyboard::IsKeyPressed(u8 keyCode) const noexcept
    {
        return _keyStates[keyCode];
    }

    Keyboard::Event Keyboard::ReadKey() noexcept
    {
        if (_keyBuffer.size() > 0)
        {
            Keyboard::Event e {_keyBuffer.front()};
            _keyBuffer.pop();
            return e;
        }
        else
        {
            return Keyboard::Event {};
        }
    }

    bool Keyboard::IsKeyEmpty() const noexcept
    {
        return _keyBuffer.empty();
    }

    u8 Keyboard::ReadChar() noexcept
    {
        if (_charBuffer.size() > 0)
        {
            u8 charCode {_charBuffer.front()};
            _charBuffer.pop();
            return charCode;
        }
        else
        {
            return 0;
        }
    }

    bool Keyboard::IsCharEmpty() const noexcept
    {
        return _charBuffer.empty();
    }

    void Keyboard::FlushKey() noexcept
    {
        _keyBuffer = std::queue<Event> {};
    }

    void Keyboard::FlushChar() noexcept
    {
        _charBuffer = std::queue<u8> {};
    }

    void Keyboard::Flush() noexcept
    {
        FlushKey();
        FlushChar();
    }

    void Keyboard::EnableAutoRepeat() noexcept
    {
        _autoRepeatEnabled = true;
    }

    void Keyboard::DisableAutoRepeat() noexcept
    {
        _autoRepeatEnabled = false;
    }

    bool Keyboard::IsAutoRepeatEnabled() const noexcept
    {
        return _autoRepeatEnabled;
    }

    void Keyboard::OnKeyPressed(u8 keycode) noexcept
    {
        _keyStates[keycode] = true;
        _keyBuffer.push(Keyboard::Event {Keyboard::Event::Type::PRESS, keycode});
        TrimBuffer(_keyBuffer);
    }

    void Keyboard::OnKeyReleased(u8 keycode) noexcept
    {
        _keyStates[keycode] = false;
        _keyBuffer.push(Keyboard::Event {Keyboard::Event::Type::RELEASE, keycode});
        TrimBuffer(_keyBuffer);
    }

    void Keyboard::OnChar(u8 charCode) noexcept
    {
        _charBuffer.push(charCode);
        TrimBuffer(_charBuffer);
    }

    void Keyboard::ClearState() noexcept
    {
        _keyStates.reset();
    }

    template <typename T>
    void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
    {
        while (buffer.size() > s_BUFFER_SIZE)
        {
            buffer.pop();
        }
    }

} // namespace drop::input