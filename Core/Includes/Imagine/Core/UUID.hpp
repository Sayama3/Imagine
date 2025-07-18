//
// Created by Sayama on 29/04/2025.
//

#pragma once

#define NULL_UUID ::Imagine::UUID::Null()

#include <spdlog/spdlog.h>
#include "UUID.forward.hpp"

namespace Imagine {
	class UUID {
		friend std::hash<Imagine::UUID>;
		friend YAML::convert<Imagine::UUID>;
		friend AssetHandle;
	public:
		static inline constexpr uint64_t Count64 = 2;
		static inline constexpr uint64_t Count32 = 4;
		static inline constexpr uint64_t Count8 = 16;

		using Array64 = std::array<uint64_t, Count64>;
		using Array32 = std::array<uint32_t, Count32>;
		using Array8 = std::array<uint8_t, Count8>;

		using DefaultArray = Array64;
	public:
		using const_iterator_64 = Array64::const_iterator;
		using const_iterator_32 = Array32::const_iterator;
		using const_iterator_8 = Array8::const_iterator;
		using const_iterator = DefaultArray::const_iterator;

	private:
		using iterator_64 = Array64::iterator;
		using iterator_32 = Array32::iterator;
		using iterator_8 = Array8::iterator;
		using iterator = DefaultArray::iterator;

	public:
		static constexpr UUID Null() { return UUID{0, 0}; }

	public:
		UUID();
		~UUID() = default;
		constexpr explicit UUID(const uint64_t id) : m_UUID1(id), m_UUID2(id) {}
		constexpr explicit UUID(const uint64_t id1, const uint64_t id2) : m_UUID1(id1), m_UUID2(id2) {}

		UUID(const UUID &) = default;
		UUID &operator=(const UUID &) = default;

		[[nodiscard]] std::string string() const;
		[[nodiscard]] std::string raw_string() const;
		[[nodiscard]] explicit operator bool() const;

	public:
		friend bool operator==(const UUID &lft, const UUID &rht);
		friend bool operator!=(const UUID &lft, const UUID &rht);
		std::strong_ordering operator<=>(const UUID &rht) const;

	public:
		// Some iterator because it's really useful to be able to iterate on the bytes of an UUID.
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;

		const_iterator_64 cbegin64() const;
		const_iterator_64 cend64() const;

		const_iterator_32 cbegin32() const;
		const_iterator_32 cend32() const;

		const_iterator_8 cbegin8() const;
		const_iterator_8 cend8() const;

	public:
		// Some iterator because it's really useful to be able to iterate on the bytes of an UUID.
		iterator begin();
		iterator end();

		iterator_64 begin64();
		iterator_64 end64();

		iterator_32 begin32();
		iterator_32 end32();

		iterator_8 begin8();
		iterator_8 end8();

	private:
		union {
			struct {uint64_t m_UUID1, m_UUID2;};
			Array64 m_Array64;
			Array32 m_Array32;
			Array8 m_Array8;
		};
	};
} // namespace Imagine

template<>
struct std::hash<Imagine::UUID> {
	std::size_t operator()(const Imagine::UUID &s) const noexcept {
		constexpr auto hasher = std::hash<uint64_t>{};

		const auto h1 = hasher(s.m_UUID1);
		const auto h2 = hasher(s.m_UUID2);

		return h1 ^ (h2 << 1);
	}
};

inline std::ostream &operator<<(std::ostream &os, const Imagine::UUID &value) {
	return os << value.string();
}

template<>
struct fmt::formatter<Imagine::UUID> : fmt::formatter<std::string> {
	auto format(Imagine::UUID value, format_context &ctx) const -> decltype(ctx.out()) {
		return format_to(ctx.out(), "{}", value.string());
	}
};
