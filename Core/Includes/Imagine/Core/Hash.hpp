//
// Created by ianpo on 07/07/2025.
//

#pragma once
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Hasher {
	struct hash128_t {
		static_assert(sizeof(UUID) == 32);

		constexpr hash128_t() :	low64{0}, high64{0} {}
		constexpr ~hash128_t() = default;
		constexpr hash128_t(const uint64_t low, const uint64_t high) : 	low64{low}, high64{high} {}

		union {
			struct { uint64_t low64, high64; };
			std::pair<uint64_t, uint64_t> pair;
			UUID uuid;
#if defined(__GNUC__)
			unsigned __int128 _u128;
#endif
		};
	};


	[[nodiscard]] uint32_t xxHash32(ConstBufferView view, uint32_t seed = 0);
	[[nodiscard]] uint64_t xxHash64(ConstBufferView view, uint64_t seed = 0);
	[[nodiscard]] hash128_t xxHash128(ConstBufferView view, uint64_t seed = 0);
}
