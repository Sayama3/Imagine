//
// Created by ianpo on 13/07/2025.
//

#pragma once

#include <sol/sol.hpp>

#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Core/TimeStep.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine {
	class ScriptingLayer;
	class LuaScript {
		friend ScriptingLayer;
	public:
		struct Log {
			enum Type {
				None,
				Error,
				Info
			};
			using double_seconds = std::chrono::duration<double, std::ratio<1>>;
			using local_time = std::chrono::zoned_time<double_seconds>;
			static local_time Now();
			Log() = default;
			Log(Type t, std::string str);
			Log(std::string str);
			~Log() = default;
			std::string time;
			std::string log;
			Type type;
		};
		enum Event : uint16_t {
			UpdateEvent,
			InputEvent,
			Count,
		};

		static inline constexpr std::string EventToString(Event e) {
			switch (e) {
				case UpdateEvent:
					return "Update";
					break;
				case InputEvent:
					return "Input";
					break;
				default:
					return "Unknown";
			}
			return "Unknown";
		}

	public:
		LuaScript();
		LuaScript(const Path &path);
		LuaScript(const std::filesystem::path &path);
		LuaScript(const LuaScript &) = delete;
		LuaScript &operator=(const LuaScript &) = delete;
		LuaScript(LuaScript &&) noexcept;
		LuaScript &operator=(LuaScript &&) noexcept;
		~LuaScript();
		void swap(LuaScript &o) noexcept;

	public:
		bool Load(const std::filesystem::path &path);
		bool TryReload();
		void Update(TimeStep ts);

	private:
		void LoadLogger();
		void LoadMathType();
		void LoadKeyboardTypes();
		void LoadKeyboardFuncs();
		void LoadMouseTypes();
		void LoadMouseFuncs();
		void LoadScene();

	public:
		template<typename ... Args>
		void Call(std::string name, Args&&... args) {
			if (m_State) {
				(*m_State)[name](std::forward<Args>(args)...);
			}
		}
	public:
		const std::filesystem::path& GetPath() const;
	private:
		Scope<sol::state> m_State;
		std::filesystem::path m_Path;
		bool m_IsValid{false};
		bool m_HardReload;
		std::array<bool, Count> m_EventsValidity{true};
		std::filesystem::file_time_type m_TimeEdited{std::filesystem::file_time_type::min()};
		LoopBackBuffer<Log, 200> m_LoggerStack;
	};

} // namespace Imagine
