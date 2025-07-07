//
// Created by Sayama on 29/04/2025.
//

#pragma once

#define NULL_UUID ::Imagine::Core::UUID::Null()

#include <spdlog/spdlog.h>
#include "UUID.forward.hpp"

namespace Imagine::Core {
	class UUID {
		friend std::hash<Imagine::Core::UUID>;
		friend YAML::convert<Imagine::Core::UUID>;
		friend AssetHandle;
	public:
		using Array64 = std::array<uint64_t, 2>;
		using Array32 = std::array<uint64_t, 4>;
		using Array8 = std::array<uint8_t, 16>;

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

	private:
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
struct fmt::formatter<Imagine::Core::UUID> : fmt::formatter<std::string> {
	auto format(Imagine::Core::UUID value, format_context &ctx) const -> decltype(ctx.out()) {
		return format_to(ctx.out(), "{}", value.string());
	}
};
