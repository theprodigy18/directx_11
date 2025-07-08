#include "input/mouse.hpp"

#include <Windows.h>

namespace drop::input
{
    std::pair<i32, i32> Mouse::GetPos() const noexcept
    {
        return {_x, _y};
    }

    i32 Mouse::GetX() const noexcept
    {
        return _x;
    }

    i32 Mouse::GetY() const noexcept
    {
        return _y;
    }

    bool Mouse::IsInWindow() const noexcept
    {
        return _isInWindow;
    }

    bool Mouse::IsLeftPressed() const noexcept
    {
        return _leftIsPressed;
    }

    bool Mouse::IsRightPressed() const noexcept
    {
        return _rightIsPressed;
    }

    Mouse::Event Mouse::Read() noexcept
    {
        if (_buffer.size() > 0)
        {
            Mouse::Event e {_buffer.front()};
            _buffer.pop();
            return e;
        }
        else
        {
            return Mouse::Event {};
        }
    }

    bool Mouse::IsEmpty() const noexcept
    {
        return _buffer.empty();
    }

    void Mouse::Flush() noexcept
    {
        _buffer = std::queue<Event> {};
    }

    void Mouse::OnMouseMove(i32 x, i32 y) noexcept
    {
        _x = x;
        _y = y;

        _buffer.push(Mouse::Event {Mouse::Event::Type::MOVE, *this});
        TrimBuffer();
    }

    void Mouse::OnMouseLeave() noexcept
    {
        _isInWindow = false;

        _buffer.push(Mouse::Event {Mouse::Event::Type::LEAVE, *this});
        TrimBuffer();
    }

    void Mouse::OnMouseEnter() noexcept
    {
        _isInWindow = true;

        _buffer.push(Mouse::Event {Mouse::Event::Type::ENTER, *this});
        TrimBuffer();
    }

    void Mouse::OnLeftPressed(i32 x, i32 y) noexcept
    {
        _leftIsPressed = true;

        _buffer.push(Mouse::Event {Mouse::Event::Type::L_PRESS, *this});
        TrimBuffer();
    }

    void Mouse::OnLeftReleased(i32 x, i32 y) noexcept
    {
        _leftIsPressed = false;

        _buffer.push(Mouse::Event {Mouse::Event::Type::L_RELEASE, *this});
        TrimBuffer();
    }

    void Mouse::OnRightPressed(i32 x, i32 y) noexcept
    {
        _rightIsPressed = true;

        _buffer.push(Mouse::Event {Mouse::Event::Type::R_PRESS, *this});
        TrimBuffer();
    }

    void Mouse::OnRightReleased(i32 x, i32 y) noexcept
    {
        _rightIsPressed = false;

        _buffer.push(Mouse::Event {Mouse::Event::Type::R_RELEASE, *this});
        TrimBuffer();
    }

    void Mouse::OnWheelUp(i32 x, i32 y) noexcept
    {
        _buffer.push(Mouse::Event {Mouse::Event::Type::WHEEL_UP, *this});
        TrimBuffer();
    }

    void Mouse::OnWheelDown(i32 x, i32 y) noexcept
    {
        _buffer.push(Mouse::Event {Mouse::Event::Type::WHEEL_DOWN, *this});
        TrimBuffer();
    }

    void Mouse::OnWheelDelta(i32 x, i32 y, i32 delta) noexcept
    {
        _wheelDelta += delta;
        // Generate events for every 120.
        if (_wheelDelta >= WHEEL_DELTA)
        {
            _wheelDelta -= WHEEL_DELTA;
            OnWheelUp(x, y);
        }
        else if (_wheelDelta <= -WHEEL_DELTA)
        {
            _wheelDelta += WHEEL_DELTA;
            OnWheelDown(x, y);
        }
    }

    void Mouse::TrimBuffer() noexcept
    {
        while (_buffer.size() > s_BUFFER_SIZE)
        {
            _buffer.pop();
        }
    }
} // namespace drop::input