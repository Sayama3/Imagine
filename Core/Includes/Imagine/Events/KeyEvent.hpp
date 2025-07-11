//
// Created by ianpo on 14/07/2023.
//

#pragma once

#include "Event.hpp"
#include "Imagine/Core/KeyCodes.hpp"

namespace Imagine {
	class KeyEvent : public Event {
	public:
		inline Key GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryInput | EventCategory::EventCategoryKeyboard)
	protected:
		inline KeyEvent(Key keycode) : m_KeyCode(keycode) {}
		inline KeyEvent(int keycode) : m_KeyCode(static_cast<Key>(keycode)) {}

		Key m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(const Key keycode, int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}
		KeyPressedEvent(const int keycode, int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }
		inline std::string string() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << static_cast<int>(m_KeyCode) << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::KeyPressed)

	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const Key keycode) : KeyEvent(keycode) {}
		KeyReleasedEvent(const int keycode) : KeyEvent(keycode) {}

		inline std::string string() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << static_cast<int>(m_KeyCode);
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::KeyReleased)
	};

	class TextTypedEvent : public Event
	{
	public:
		explicit TextTypedEvent(const char *text) : m_TextTyped(text) {}
		explicit TextTypedEvent(std::string text) : m_TextTyped(std::move(text)) {}

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryInput | EventCategory::EventCategoryKeyboard)
		EVENT_CLASS_TYPE(EventType::TextTyped)

		inline std::string string() const override
		{
			std::stringstream ss;
			ss << "TextTypedEvent: '" << m_TextTyped << "'";
			return ss.str();
		}

		inline const std::string& GetTextTyped() const { return m_TextTyped; }
	private:
		std::string m_TextTyped;

	};

}
