//
// Created by ianpo on 14/07/2023.
//

#pragma once

#include "Imagine/Core/Macros.hpp"

#define MGN_DISPATCH_FALSE(fn) [this](auto &&...args) -> bool { this->fn(std::forward<decltype(args)>(args)...); return false;}
#define MGN_DISPATCH_TRUE(fn) [this](auto &&...args) -> bool { this->fn(std::forward<decltype(args)>(args)...); return true;}
#define MGN_DISPATCH(fn) MGN_BIND_EVENT_FN(fn)

namespace Imagine {

	enum class EventType : uint32_t {
		None = 0,
		WindowClose, WindowResize, WindowMinify, WindowFocus, WindowLostFocus, WindowMoved, WindowDropFile,
		AppTick, AppUpdate, AppRender,
		ImGui,
		KeyPressed, KeyReleased, TextTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
	};

	enum EventCategory : uint32_t {
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
	};

#define EVENT_CLASS_TYPE(type)                                                  \
	static EventType GetStaticType() { return type; }                           \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char *GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return category; }

	class Event {
		friend class EventDispatcher;

	public:
		bool m_Handled = false;

		virtual ~Event() = default;
		virtual EventType GetEventType() const = 0;
		virtual const char *GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string string() const { return GetName(); }
		inline bool IsInCategory(EventCategory category) {
			return GetCategoryFlags() & category;
		}
	};

	class EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool>(T &);

	private:
		Event &m_Event;

	public:
		inline EventDispatcher(Event &event) :
			m_Event(event) {
		}

		/// Function used to handle an event with whatever thingy u pass that can be used like a function, that the EventType reference and return a boolean.
		/// @tparam EventType The Event Type we checks
		/// @tparam FuncType is deduced by the compiler. It should be some type of `bool(*functionPtr)(EventType& e)` function (in C-Style but accept anything.). The boolean return whether the dispatch has handled the event and therefore should not be passed down to other event.
		/// @param func the function used to handle the EventType event
		/// @return Whether the event was of said EventType.
		template<typename EventType, typename FuncType>
		inline bool Dispatch(const FuncType &func) {
			if (m_Event.GetEventType() == EventType::GetStaticType()) {
				m_Event.m_Handled = func(static_cast<EventType &>(m_Event));
				return true;
			}
			return false;
		}
	};

} // namespace Imagine


inline std::ostream &operator<<(std::ostream &os, const Imagine::Event &e) {
	return os << e.string();
}

template<>
struct fmt::formatter<Imagine::Event> : fmt::formatter<std::string>
{
	auto format(const Imagine::Event& value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", value.string());
	}
};