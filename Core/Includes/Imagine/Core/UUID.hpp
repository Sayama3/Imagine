//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "TypeHelper.hpp"

namespace Imagine::Core {

	class UUID {
	public:
		static constexpr UUID Null() { return UUID{0, 0}; }

		template<typename T>
		inline static UUID FromType() {
			return UUID{typeid(T).hash_code()};
		}

	public:
		UUID();
		constexpr explicit UUID(const uint64_t id) :
			m_UUID1(id), m_UUID2(id) {}
		constexpr explicit UUID(const uint64_t id1, const uint64_t id2) :
			m_UUID1(id1), m_UUID2(id2) {}

		UUID(const UUID &) = default;
		UUID &operator=(const UUID &) = default;

		[[nodiscard]] std::string string() const;
		[[nodiscard]] explicit operator bool() const;

	public:
		friend bool operator==(const UUID &lft, const UUID &rht);
		friend bool operator!=(const UUID &lft, const UUID &rht);

	private:
		uint64_t m_UUID1, m_UUID2;
	};
} // namespace Imagine::Core

template<>
struct std::hash<Imagine::Core::UUID> {
	std::size_t operator()(const Imagine::Core::UUID &s) const noexcept {
		static_assert(sizeof(Imagine::Core::UUID) == (sizeof(uint64_t[2])));

		const uint64_t v1 = reinterpret_raw<uint64_t[2]>(s)[0];
		const uint64_t v2 = reinterpret_raw<uint64_t[2]>(s)[1];

		constexpr auto hasher = std::hash<uint64_t>{};

		const auto h1 = hasher(v1);
		const auto h2 = hasher(v2);

		return h1 ^ (h2 << 1);
	}
};

inline std::ostream &operator<<(std::ostream &os, const Imagine::Core::UUID &value) {
	return os << value.string();
}
