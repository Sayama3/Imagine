//
// Created by ianpo on 12/05/2025.
//

#pragma once

namespace Imagine::Core::MemoryHelper {
	template<typename Type>
	inline static void c_swap_memory(Type *id1, Type *id2) {
		//TODO: Optimize ?
		Type *tmp = reinterpret_cast<Type*>(malloc(sizeof(Type)));
		memcpy(tmp, id1, sizeof(Type));
		memcpy(id1, id2, sizeof(Type));
		memcpy(id2, tmp, sizeof(Type));
		free(tmp);
	}
	template<typename Type>
	inline static void c_swap_memory(Type& id1, Type& id2) {
		c_swap_memory(&id1, &id2);
	}
}
