//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "TypeHelper.hpp"

namespace Imagine::Core {

	class UUID {
		UUID();
		UUID(const UUID&) = default;
		UUID& operator=(const UUID&) = default;

		[[nodiscard]] std::string string() const;
		[[nodiscard]] explicit inline operator bool() const {return m_UUID != 0;}
	public:
		friend bool operator ==(const UUID& lft, const UUID& rht);
		friend bool operator !=(const UUID& lft, const UUID& rht);
	private:
		uint64_t m_UUID;
	};
}

template<>
struct std::hash<Imagine::Core::UUID>
{
	std::size_t operator()(const Imagine::Core::UUID& s) const noexcept
	{
		static_assert(sizeof(Imagine::Core::UUID) == sizeof(uint64_t));
		return std::hash<uint64_t>{}(reinterpret_raw<uint64_t>(s));
	}
};