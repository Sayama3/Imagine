//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Core/UUID.hpp"
#include <spdlog/spdlog.h>

#define NULL_ASSET_HANDLE ::Imagine::AssetHandle::Null()

namespace Imagine {

	struct AssetHandle {
		friend std::hash<Imagine::AssetHandle>;
		static constexpr AssetHandle Null() { return AssetHandle{0, 0}; }
	public:
		AssetHandle();
		~AssetHandle() = default;
		AssetHandle(const AssetHandle&) = default;
		AssetHandle &operator=(const AssetHandle &) = default;
		constexpr explicit AssetHandle(const UUID id) : m_Id(id) {}
	private:
		constexpr explicit AssetHandle(const uint64_t a, const uint64_t b) : m_Id{a,b} {}
	public:
		[[nodiscard]] friend bool operator==(const AssetHandle &lft, const AssetHandle &rht);
		[[nodiscard]] friend bool operator!=(const AssetHandle &lft, const AssetHandle &rht);
		[[nodiscard]] std::strong_ordering operator<=>(const AssetHandle &rht) const;

		[[nodiscard]] std::string string() const;
		[[nodiscard]] explicit operator bool() const;
	public:
		[[nodiscard]] UUID GetID() const;
	private:
		UUID m_Id{};
	};
} // namespace Imagine


template<>
struct std::hash<Imagine::AssetHandle> {
	std::size_t operator()(const Imagine::AssetHandle &s) const noexcept {
		return std::hash<Imagine::UUID>{}(s.m_Id);
	}
};

inline std::ostream &operator<<(std::ostream &os, const Imagine::AssetHandle &value) {
	return os << value.string();
}

template<>
struct fmt::formatter<Imagine::AssetHandle> : fmt::formatter<std::string>
{
	auto format(Imagine::AssetHandle value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", value.string());
	}
};
