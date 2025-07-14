//
// Created by ianpo on 13/07/2025.
//

#include "Imagine/Scripting/LuaScript.hpp"
#include <format>

namespace Imagine {
	LuaScript::Log::local_time LuaScript::Log::Now() {
		return local_time{
				std::chrono::current_zone(),
				std::chrono::system_clock::now(),
		};
	}
	LuaScript::Log::Log(Type t, std::string str) :
		type(t), log(std::move(str)) {
		time = std::format("{:%T}", Now());
	}

	LuaScript::Log::Log(std::string str) :
		type(Info), log(std::move(str)) {
		time = std::format("{:%T}", Now());
	}

	LuaScript::LuaScript() {
		m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);
		LoadLogger();
		m_State.set_function("print", [this](sol::variadic_args args) {
			std::string str;
			for (auto arg: args) {
				str += m_State["tostring"](arg.get<sol::object>()).get<std::string>();
			}
		});
	}

	LuaScript::LuaScript(const Path &path) :
		m_Path(path.GetFullPath()) {
		m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);
		LoadLogger();
		Load(path.GetFullPath());
	}

	LuaScript::LuaScript(const std::filesystem::path &path) :
		m_Path(path) {
		m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);
		LoadLogger();
		Load(path);
	}

	LuaScript::LuaScript(LuaScript &&o) noexcept :
		m_State(std::move(o.m_State)), m_Path(std::move(o.m_Path)), m_TimeEdited(std::move(o.m_TimeEdited)) {
	}
	LuaScript &LuaScript::operator=(LuaScript &&o) noexcept {
		swap(o);
		return *this;
	}

	LuaScript::~LuaScript() = default;

	void LuaScript::swap(LuaScript &o) noexcept {
		std::swap(m_State, o.m_State);
		std::swap(m_Path, o.m_Path);
		std::swap(m_TimeEdited, o.m_TimeEdited);
	}

	bool LuaScript::Load(const std::filesystem::path &path) {
		m_LoggerStack.push_front({Log::None, "============ Reload at %s ============"});
		const auto result = m_State.safe_script_file(path.string(), sol::script_pass_on_error);
		m_Path = path;
		m_TimeEdited = std::filesystem::last_write_time(m_Path);
		m_IsValid = result.valid();
		if (!m_IsValid) {
			sol::error err = result;
			m_LoggerStack.push_front({Log::Error, err.what()});
			MGN_CORE_ERROR("[Lua] Failed to load script {0}\n{1}", path, err.what());
			return false;
		}
		else {
			for (uint16_t i = 0; i < Count; ++i) {
				const Event e = (Event) i;
				sol::protected_function eventFunc = m_State[EventToString(e)];
				m_EventsValidity[i] = eventFunc.valid();
			}
		}
		return m_IsValid;
	}

	bool LuaScript::TryReload() {
		if (std::filesystem::last_write_time(m_Path) > m_TimeEdited) {
			return Load(m_Path);
		}
		return false;
	}

	void LuaScript::Update(TimeStep ts) {
		if (!m_IsValid) return;
		if (!m_EventsValidity[UpdateEvent]) return;

		sol::protected_function eventFunc = m_State[EventToString(UpdateEvent)];
		if (eventFunc) {
			auto result = eventFunc(ts.GetSeconds());
			if (!result.valid()) {
				m_EventsValidity[UpdateEvent] = false;
				const sol::error err = result;
				m_LoggerStack.push_front({Log::Error, err.what()});
				MGN_CORE_ERROR("[Lua] Failed to run Update {0}\n{1}", m_Path, err.what());
			}
			// m_State.
		}
	}
	void LuaScript::LoadLogger() {
		// m_LoggerStack.reserve(1000);
		m_State.set_function("print", [this](sol::variadic_args args) {
			std::string str;
			for (auto arg: args) {
				str += m_State["tostring"](arg.get<sol::object>()).get<std::string>() + "\t";
			}
			m_LoggerStack.push_front({Log::Info, std::move(str)});
		});
	}

	void LuaScript::LoadKeyboard() {
		// m_State.
	}
	const std::filesystem::path &LuaScript::GetPath() const {
		return m_Path;
	}
} // namespace Imagine
