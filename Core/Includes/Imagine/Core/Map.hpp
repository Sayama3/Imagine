//
// Created by Sayama on 20/05/2025.
//

#pragma once

#include <concepts>
#include <functional>
#include "SparseSet.hpp"

namespace Imagine {
	// Declaration of the concept �Hashable�, which is satisfied by any type �T�
	// such that for values �a� of type �T�, the expression std::hash<T>{}(a)
	// compiles and its result is convertible to std::size_t
	template<typename T>
	concept Hashable = requires(T a) {
		{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
	};

	template<Hashable Key, typename Value>
	class HashMap {
	private:
		SparseSet<Value> m_Data;
	};

} // namespace Imagine
