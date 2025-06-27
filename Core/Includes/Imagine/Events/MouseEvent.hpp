//
// Created by ianpo on 14/07/2023.
//

#pragma once

#include "Event.hpp"
#include "Imagine/Core/MouseButtonCodes.hpp"

namespace Imagine::Core {

    class MouseMovedEvent : public Event
    {
    public:
        inline MouseMovedEvent(const float x, const float y, float const xMov, const float yMov) : m_MouseX(x), m_MouseY(y), m_MouseMovX(xMov), m_MouseMovY(yMov) {}

        inline float GetX() const { return m_MouseX; }
        inline float GetY() const { return m_MouseY; }
        inline float GetXMovement() const { return m_MouseMovX; }
        inline float GetYMovement() const { return m_MouseMovY; }

        inline std::string string() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput)
    private:
        float m_MouseX, m_MouseY;
        float m_MouseMovX, m_MouseMovY;
    };

    class MouseScrolledEvent : public Event
    {
    public:
        inline MouseScrolledEvent(const float xOffset, const float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

        inline float GetXOffset() const { return m_XOffset; }
        inline float GetYOffset() const { return m_YOffset; }

        inline std::string string() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput)
    private:
        float m_XOffset, m_YOffset;
    };

    class MouseButtonEvent : public Event
    {
    public:
        inline Mouse GetMouseButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput | EventCategory::EventCategoryMouseButton)
    protected:
        inline MouseButtonEvent(const Mouse button) : m_Button(button) {}
        inline MouseButtonEvent(const int button) : m_Button(static_cast<Mouse>(button)) {}

        Mouse m_Button;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        inline MouseButtonPressedEvent(const int button) : MouseButtonEvent(button) {}
        inline MouseButtonPressedEvent(const Mouse button) : MouseButtonEvent(button) {}

        inline std::string string() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << static_cast<int>(m_Button);
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        inline MouseButtonReleasedEvent(const int button) : MouseButtonEvent(button) {}
        inline MouseButtonReleasedEvent(const Mouse button) : MouseButtonEvent(button) {}

        inline std::string string() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << static_cast<int>(m_Button);
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseButtonReleased)
    };

}
