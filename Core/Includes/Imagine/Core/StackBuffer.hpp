//
// Created by Iannis on 21/05/2025.
//

#pragma once

#include "MemoryHelper.hpp"

namespace Imagine::Core {
	
	template<typename UnsignedInteger, UnsignedInteger ByteCapacity>
	class StackBuffer {
		static_assert(std::is_unsigned_v<UnsignedInteger> == true);
	public:
		StackBuffer() = default;
		~StackBuffer() = default;
	public:
		void* get() { return m_Buffer; }

		const void* get() const { return m_Buffer; }

		template<typename T>
		T* get() { return reinterpret_cast<T*>(m_Buffer); }

		template<typename T>
		const T* get() const { return reinterpret_cast<const T*>(m_Buffer); }
		
		set(const void* data, const UnsignedInteger size) {
			memcpy(m_Buffer, data, std::min(size, ByteCapacity));
		}

		void clear() {
			memset(m_Buffer, 0, ByteCapacity);
		}
	private:
		UnsignedInteger Count{0};
		uint8_t m_Buffer[ByteCapacity];
	};
}