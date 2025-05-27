//
// Created by Sayama on 29/04/2025.
//

#pragma once

namespace Imagine::Core {

	template<typename T2, typename T1>
	inline static T2& reinterpret_raw(T1& value)
	{
		static_assert(sizeof(T2) == sizeof(T1));
		return *reinterpret_cast<T2*>(&value);
	}

	template<typename T2, typename T1>
	inline static const T2& reinterpret_raw(const T1& value)
	{
		static_assert(sizeof(T2) == sizeof(T1));
		return *reinterpret_cast<const T2*>(&value);
	}

	// helper type for the visitor #4
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

	// explicit deduction guide (not needed as of C++20)
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}