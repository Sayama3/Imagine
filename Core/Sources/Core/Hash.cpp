//
// Created by ianpo on 07/07/2025.
//

#include "Imagine/Core/Hash.hpp"
#include <xxhash.h>

namespace Imagine::Hasher {
	uint32_t xxHash32(const Core::ConstBufferView view, const uint32_t seed /*= 0*/) {
		const uint32_t hash = XXH32(view.Get(), view.Size(), seed);
		return hash;
	}

	uint64_t xxHash64(const Core::ConstBufferView view, const uint64_t seed /*= 0*/) {
		const uint64_t hash = XXH3_64bits_withSeed(view.Get(), view.Size(), seed);
		return hash;
	}

	hash128_t xxHash128(const Core::ConstBufferView view, const uint64_t seed /*= 0*/) {
		union {
			hash128_t mgnSeed{0,0};
			XXH128_hash_t xxxSeed;
		};
		xxxSeed = XXH3_128bits_withSeed(view.Get(), view.Size(), seed);
		return mgnSeed;
	}
} // namespace Imagine::Hasher
