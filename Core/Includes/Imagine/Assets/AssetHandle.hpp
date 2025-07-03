//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Core/UUID.hpp"
#include <spdlog/spdlog.h>

#define NULL_ASSET_HANDLE ::Imagine::Core::AssetHandle::Null()

namespace Imagine::Core {

	struct AssetHandle {
		friend std::hash<Imagine::Core::AssetHandle>;
		static constexpr AssetHandle Null() { return AssetHandle{0, 0}; }
	public:
		AssetHandle() = default;
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
} // namespace Imagine::Core


template<>
struct std::hash<Imagine::Core::AssetHandle> {
	std::size_t operator()(const Imagine::Core::AssetHandle &s) const noexcept {
		return std::hash<Imagine::Core::UUID>{}(s.m_Id);
	}
};

inline std::ostream &operator<<(std::ostream &os, const Imagine::Core::AssetHandle &value) {
	return os << value.string();
}

template<>
struct fmt::formatter<Imagine::Core::AssetHandle> : fmt::formatter<std::string>
{
	auto format(Imagine::Core::AssetHandle value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", value.string());
	}
};
