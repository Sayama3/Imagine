//
// Created by ianpo on 13/07/2025.
//

#pragma once

#include <sol/sol.hpp>

#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Core/TimeStep.hpp"

namespace Imagine {

class LuaScript {
public:
	LuaScript();
	LuaScript(const Path& path);
	LuaScript(const std::filesystem::path& path);
	LuaScript(const LuaScript&) = delete;
	LuaScript& operator=(const LuaScript&) = delete;
	LuaScript(LuaScript&&) noexcept;
	LuaScript& operator=(LuaScript&&) noexcept;
	~LuaScript();
	void swap(LuaScript& o) noexcept;
public:
	bool Load(const std::filesystem::path& path);
	bool TryReload();
	void Update(TimeStep ts);
private:
	sol::state m_State;
	std::filesystem::path m_Path;
	std::filesystem::file_time_type m_TimeEdited{std::filesystem::file_time_type::min()};
};

} // Imagine
