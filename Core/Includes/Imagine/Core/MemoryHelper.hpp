//
// Created by ianpo on 12/05/2025.
//

#pragma once

namespace Imagine::MemoryHelper {
	inline static void c_swap_memory(void *id1, void *id2, const uint64_t size) {
		// TODO: Optimize ?
		void *tmp = malloc(size);
		memcpy(tmp, id1, size);
		memmove(id1, id2, size);
		memcpy(id2, tmp, size);
		free(tmp);
	}

	template<typename Type>
	inline static void c_swap_memory(Type *id1, Type *id2) {
		c_swap_memory(id1, id2, sizeof(Type));
	}

	template<typename Type>
	inline static void c_swap_memory(Type &id1, Type &id2) {
		c_swap_memory(&id1, &id2);
	}
} // namespace Imagine::MemoryHelper
