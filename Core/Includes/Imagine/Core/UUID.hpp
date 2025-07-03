//
// Created by Sayama on 29/04/2025.
//

#pragma once

#define NULL_UUID ::Imagine::Core::UUID::Null()

#include <spdlog/spdlog.h>

namespace Imagine::Core {

	struct AssetHandle;
	class UUID {
		friend std::hash<Imagine::Core::UUID>;
		friend AssetHandle;
	public:
		static constexpr UUID Null() { return UUID{0, 0}; }

		template<typename T>
		inline static UUID FromType() {
			return UUID{typeid(T).hash_code()};
		}

	public:
		UUID();
		~UUID() = default;
		constexpr explicit UUID(const uint64_t id) : m_UUID1(id), m_UUID2(id) {}
		constexpr explicit UUID(const uint64_t id1, const uint64_t id2) : m_UUID1(id1), m_UUID2(id2) {}

		UUID(const UUID &) = default;
		UUID &operator=(const UUID &) = default;

		[[nodiscard]] std::string string() const;
		[[nodiscard]] explicit operator bool() const;

	public:
		friend bool operator==(const UUID &lft, const UUID &rht);
		friend bool operator!=(const UUID &lft, const UUID &rht);
		std::strong_ordering operator<=>(const UUID &rht) const;

	private:
		uint64_t m_UUID1, m_UUID2;
	};
} // namespace Imagine::Core

template<>
struct std::hash<Imagine::Core::UUID> {
	std::size_t operator()(const Imagine::Core::UUID &s) const noexcept {
		constexpr auto hasher = std::hash<uint64_t>{};

		const auto h1 = hasher(s.m_UUID1);
		const auto h2 = hasher(s.m_UUID2);

		return h1 ^ (h2 << 1);
	}
};

inline std::ostream &operator<<(std::ostream &os, const Imagine::Core::UUID &value) {
	return os << value.string();
}

template<>
struct fmt::formatter<Imagine::Core::UUID> : fmt::formatter<std::string>
{
	auto format(Imagine::Core::UUID value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", value.string());
	}
};

