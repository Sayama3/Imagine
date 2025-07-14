//
// Created by ianpo on 13/07/2025.
//

#include "Imagine/Scripting/LuaScript.hpp"

namespace Imagine {
	LuaScript::LuaScript() {
		m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);
	}

	LuaScript::LuaScript(const Path &path) {
		m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);
		Load(path.GetFullPath());
	}

	LuaScript::LuaScript(const std::filesystem::path &path) {
		m_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);
		Load(path);
	}

	LuaScript::LuaScript(LuaScript && o) noexcept : m_State(std::move(o.m_State)), m_Path(std::move(o.m_Path)), m_TimeEdited(std::move(o.m_TimeEdited)) {
	}
	LuaScript &LuaScript::operator=(LuaScript && o) noexcept {
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
		auto result = m_State.script_file(path.string());
		if (result.valid()) {
			m_Path = path;
			m_TimeEdited = std::filesystem::last_write_time(m_Path);
			return true;
		}
		else {
			sol::error err = result;
			MGN_CORE_ERROR("[Lua] Failed to load script {0}\n{1}", path, err.what());
			return false;
		}
	}

	bool LuaScript::TryReload() {
		std::filesystem::file_time_type ftt = std::filesystem::last_write_time(m_Path);
		if (ftt > m_TimeEdited) {
			return Load(m_Path);
		}
		return false;
	}

	void LuaScript::Update(TimeStep ts) {
		auto result = m_State["Update"](ts.GetSeconds());
		if (!result.valid()) {
			sol::error err = result;
			MGN_CORE_ERROR("[Lua] Failed to run Update {0}\n{1}", m_Path, err.what());
		}
	}
} // namespace Imagine
