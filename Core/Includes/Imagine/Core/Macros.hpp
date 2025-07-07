//
// Created by ianpo on 16/07/2023.
//

#pragma once

#include "PlatformDetection.hpp"

#ifndef MGN_NO_LOG
#include "Logger.hpp"
#endif

#ifndef IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM 1
#endif

#define MGN_CONCAT(x, y) x##y
#define MGN_COMBINE(x, y) MGN_CONCAT(x, y)

// Defining the "_USE_MATH_DEFINES" for visual studio in the main hpp
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#ifndef __FUNCSIG__
#ifdef __PRETTY_FUNCTION__
#define __FUNCSIG__ __PRETTY_FUNCTION__
#else
#define __FUNCSIG__ __FUNCTION__
#endif
#endif

#ifdef MGN_USE_SIMPLE_FUNCTION
#define MGN_FUNC __FUNCTION__
#else
#define MGN_FUNC __FUNCSIG__
#endif

#define MGN_BIND_FN(fn) [](auto &&...args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }
#define MGN_BIND_EVENT_FN(fn) [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
// deprecated. Use MGN_BIND_EVENT_FN
#define BIND_EVENT_FN(fn) MGN_BIND_EVENT_FN(fn)
// #define BIND_EVENT_FN(func) [this](auto && PH1) { func(std::forward<decltype(PH1)>(PH1)); }
// #define BIND_EVENT_FN(func, ...)	std::bind(&func, this, __VA_ARGS__)
// #define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#ifdef MGN_DEBUG
#ifndef MGN_USE_ASSERT
#define MGN_USE_ASSERT 1
#endif

#ifndef MGN_PROFILING
#define MGN_PROFILING 1
#endif
#endif

#ifndef MGN_DONT_PROFILE
#ifndef MGN_PROFILING
#define MGN_PROFILING 1
#endif
#endif

#if (_MSC_VER && !__INTEL_COMPILER) || (__MINGW32__ || __MINGW64__)
#define MGN_BREAK() __debugbreak();
#elif _POSIX
#include <csignal>
#define MGN_BREAK() std::raise(SIGTRAP)
#else
#define MGN_BREAK()
#endif

#define MGN_EXPAND_MACRO(x) x
#define MGN_STRINGIFY_MACRO(x) #x

#if MGN_USE_ASSERT
// Fetched from https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/Base.h
// Auto expand macro with optional message.

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define MGN_INTERNAL_ASSERT_IMPL(type, check, msg, ...) do { if(!(check)) { MGN##type##ERROR(msg, __VA_ARGS__); MGN_BREAK(); } } while (false)
#define MGN_INTERNAL_ASSERT_WITH_MSG(type, check, ...) MGN_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define MGN_INTERNAL_ASSERT_NO_MSG(type, check) MGN_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", MGN_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define MGN_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define MGN_INTERNAL_ASSERT_GET_MACRO(...) MGN_EXPAND_MACRO( MGN_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, MGN_INTERNAL_ASSERT_WITH_MSG, MGN_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define MGN_CASSERT(...) MGN_EXPAND_MACRO( MGN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define MGN_CORE_CASSERT(...) MGN_EXPAND_MACRO( MGN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )

#define MGN_MASSERT(...) MGN_EXPAND_MACRO( MGN_INTERNAL_ASSERT_IMPL(_, __VA_ARGS__) )
#define MGN_CORE_MASSERT(...) MGN_EXPAND_MACRO( MGN_INTERNAL_ASSERT_IMPL(_CORE_, __VA_ARGS__) )

#else

#define MGN_CASSERT(...)
#define MGN_CORE_CASSERT(...)
#define MGN_MASSERT(...)
#define MGN_CORE_MASSERT(...)

#endif

#if MGN_DEBUG
#define MGN_CORE_CHECK(condition, ...)	\
	while (!(condition)) {				\
		MGN_CORE_WARNING(__VA_ARGS__);	\
		break;							\
	}
#define MGN_CHECK(condition, ...)	\
	while (!(condition)) {			\
		MGN_WARNING(__VA_ARGS__);	\
		break;						\
	}
#define MGN_CORE_CHECK_ERROR(condition, ...)	\
	while (!(condition)) {						\
		MGN_CORE_ERROR(__VA_ARGS__);			\
		break;									\
	}
#define MGN_CHECK_ERROR(condition, ...) \
	while (!(condition)) {				\
		MGN_ERROR(__VA_ARGS__);         \
		break; 							\
	}
#else
#define MGN_CORE_CHECK(condition, ...)
#define MGN_CHECK(condition, ...)
#define MGN_CORE_CHECK_ERROR(condition, ...)
#define MGN_CHECK_ERROR(condition, ...)
#endif

#ifndef BIT
#define BIT(X) (1 << X)
#endif

#ifndef CHECK_BIT
/// Macro used to check a certain bit position. For exemple, if the number "4" as the byte number 2 checked.
#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#endif

// Encode a version in a single uint32_t. Stollen from <vulkan/vulkan.h>
#define MGN_MAKE_VERSION(major, minor, patch) ((((uint32_t) (major)) << 22U) | (((uint32_t) (minor)) << 12U) | ((uint32_t) (patch)))

// Fetch the Major Version in a single uint32_t. Stollen from <vulkan/vulkan.h>
#define MGN_VERSION_MAJOR(version) ((uint32_t) (version) >> 22U)
// Fetch the Minor Version in a single uint32_t. Stollen from <vulkan/vulkan.h>
#define MGN_VERSION_MINOR(version) (((uint32_t) (version) >> 12U) & 0x3FFU)
// Fetch the Patch Version in a single uint32_t. Stollen from <vulkan/vulkan.h>
#define MGN_VERSION_PATCH(version) ((uint32_t) (version) & 0xFFFU)

#ifndef MGN_NO_LOG

#define MGN_CORE_LOG_DEBUG(...) ::Imagine::Core::Log::GetCoreLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::debug, __VA_ARGS__)
#define MGN_CORE_TRACE(...) ::Imagine::Core::Log::GetCoreLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::trace, __VA_ARGS__)
#define MGN_CORE_INFO(...) ::Imagine::Core::Log::GetCoreLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::info, __VA_ARGS__)
#define MGN_CORE_WARNING(...) ::Imagine::Core::Log::GetCoreLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::warn, __VA_ARGS__)
#define MGN_CORE_WARN(...) ::Imagine::Core::Log::GetCoreLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::warn, __VA_ARGS__)
#define MGN_CORE_ERROR(...) ::Imagine::Core::Log::GetCoreLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::err, __VA_ARGS__)
#define MGN_CORE_CRITICAL(...) ::Imagine::Core::Log::GetCoreLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::critical, __VA_ARGS__)


#define MGN_LOG_DEBUG(...) ::Imagine::Core::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::debug, __VA_ARGS__)
#define MGN_TRACE(...) ::Imagine::Core::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::trace, __VA_ARGS__)
#define MGN_INFO(...) ::Imagine::Core::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::info, __VA_ARGS__)
#define MGN_WARNING(...) ::Imagine::Core::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::warn, __VA_ARGS__)
#define MGN_WARN(...) ::Imagine::Core::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::warn, __VA_ARGS__)
#define MGN_ERROR(...) ::Imagine::Core::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::err, __VA_ARGS__)
#define MGN_CRITICAL(...) ::Imagine::Core::Log::GetClientLogger()->log(spdlog::source_loc{__FILE__, __LINE__, MGN_FUNC}, spdlog::level::critical, __VA_ARGS__)

#else

#define MGN_CORE_DEBUG(...)
#define MGN_CORE_TRACE(...)
#define MGN_CORE_INFO(...)
#define MGN_CORE_WARNING(...)
#define MGN_CORE_WARN(...)
#define MGN_CORE_ERROR(...)
#define MGN_CORE_CRITICAL(...)


#define MGN_DEBUG(...)
#define MGN_TRACE(...)
#define MGN_INFO(...)
#define MGN_WARNING(...)
#define MGN_WARN(...)
#define MGN_ERROR(...)
#define MGN_CRITICAL(...)

#endif
