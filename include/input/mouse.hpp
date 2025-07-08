#pragma once

#include <queue>

namespace drop::platform
{

    class Window;
} // namespace drop::platform

namespace drop::input
{
    class Mouse
    {
        friend class platform::Window;

    public:
        class Event
        {
        public:
            enum class Type
            {
                L_PRESS,
                L_RELEASE,
                R_PRESS,
                R_RELEASE,
                WHEEL_UP,
                WHEEL_DOWN,
                MOVE,
                ENTER,
                LEAVE,
                INVALID
            };

        private:
            Type _type {Type::INVALID};
            bool _leftIsPressed {false};
            bool _rightIsPressed {false};
            i32  _x {0}, _y {0};

        public:
            Event() noexcept
                : _type {Type::INVALID}, _leftIsPressed {false}, _rightIsPressed {false},
                  _x {0}, _y {0} { }

            Event(Type type, const Mouse& parent) noexcept
                : _type {type},
                  _leftIsPressed {parent._leftIsPressed}, _rightIsPressed {parent._rightIsPressed},
                  _x {parent._x}, _y {parent._y} { }

            bool IsValid() const noexcept
            {
                return _type != Type::INVALID;
            }

            Type GetType() const noexcept
            {
                return _type;
            }

            std::pair<i32, i32> GetPos() const noexcept
            {
                return {_x, _y};
            }

            i32 GetX() const noexcept
            {
                return _x;
            }

            i32 GetY() const noexcept
            {
                return _y;
            }

            bool IsLeftPressed() const noexcept
            {
                return _leftIsPressed;
            }

            bool IsRightPressed() const noexcept
            {
                return _rightIsPressed;
            }
        };

    public:
        Mouse()                        = default;
        Mouse(const Mouse&)            = delete; // Copy Constructor.
        Mouse& operator=(const Mouse&) = delete; // Copy Assignment Operator.

        std::pair<i32, i32> GetPos() const noexcept;
        i32                 GetX() const noexcept;
        i32                 GetY() const noexcept;
        bool                IsInWindow() const noexcept;
        bool                IsLeftPressed() const noexcept;
        bool                IsRightPressed() const noexcept;
        Mouse::Event        Read() noexcept;
        bool                IsEmpty() const noexcept;
        void                Flush() noexcept;

    private:
        void OnMouseMove(i32 x, i32 y) noexcept;
        void OnMouseLeave() noexcept;
        void OnMouseEnter() noexcept;
        void OnLeftPressed(i32 x, i32 y) noexcept;
        void OnLeftReleased(i32 x, i32 y) noexcept;
        void OnRightPressed(i32 x, i32 y) noexcept;
        void OnRightReleased(i32 x, i32 y) noexcept;
        void OnWheelUp(i32 x, i32 y) noexcept;
        void OnWheelDown(i32 x, i32 y) noexcept;
        void OnWheelDelta(i32 x, i32 y, i32 delta) noexcept;
        void TrimBuffer() noexcept;

        static constexpr u32 s_BUFFER_SIZE {16};
        i32                  _x {0}, _y {0};
        bool                 _leftIsPressed {false};
        bool                 _rightIsPressed {false};
        bool                 _isInWindow {false};
        i32                  _wheelDelta {0};
        std::queue<Event>    _buffer;
    };
} // namespace drop::input