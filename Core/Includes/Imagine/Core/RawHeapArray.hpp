//
// Created by ianpo on 12/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/MemoryHelper.hpp"

namespace Imagine::Core {


	/**
	 * This heap array is a dynamically resizable array on the heap that handle allocation and deallocation of memory
	 * for a data whose type is fixed but only known at runtime. Example may include, a type created by a scripting engine
	 * whose type and therefore size is only known at runtime.
	 */
	template<typename UnsignedInteger = uint32_t>
	class RawHeapArray {
	public:
		struct HeapArrayElement {
			RawHeapArray* const array{nullptr};
			UnsignedInteger index;
			template<typename T>
			T& As() {
#ifdef MGN_DEBUG
				MGN_CORE_ASSERT(array, "The array is not initialized.");
				MGN_CORE_ASSERT(sizeof(T) == array->DataSize, "The DataSize is {} instead of the {} of the type {}", array->DataSize, sizeof(T), typeid(T).name());
#endif
				return *reinterpret_cast<T*>(&array.get(index));
			}
		};
	public:
		/**
		 * Construct the RawHeapArray and set the data size that will be used for the rest of it's lifetime.
		 * @param dataSize The size of the component the array will use. Can only be set once.
		 */
		explicit RawHeapArray(const UnsignedInteger dataSize) : DataSize(dataSize) {}
		RawHeapArray(const UnsignedInteger dataSize, const UnsignedInteger capacity) : DataSize(dataSize) { reserve(capacity); }
		~RawHeapArray() {
			free(data);
			data = nullptr;
			Count = 0;
			Capacity = 0;
		}
	private:
		void c_swap_elements(const UnsignedInteger id1, const UnsignedInteger id2)
		{
			MGN_CORE_ASSERT(id1 < Capacity, "THe index {} is out of bound.", id1);
			MGN_CORE_ASSERT(id2 < Capacity, "THe index {} is out of bound.", id2);
			MemoryHelper::c_swap_memory(&data[id1], &data[id2], DataSize);
		}
		void reallocate_and_copy(const UnsignedInteger size)
		{
			void* tmpBuffer = malloc(DataSize * size);
			if (data)
			{
				memcpy(tmpBuffer, data, std::min(Count, size)*DataSize);
			}
			std::swap(data, tmpBuffer);
			free(tmpBuffer);
			Capacity = size;
		}
		void reallocate_if_necessary(const UnsignedInteger minimumCapacity)
		{
			if (Capacity < minimumCapacity)
			{
				reallocate_and_copy(minimumCapacity);
			}
		}
	public:
		[[nodiscard]] UnsignedInteger size() const {return Count;}
		[[nodiscard]] UnsignedInteger capacity() const {return Capacity;}
		[[nodiscard]] bool empty() const {return Count == 0;}

		void reserve(const UnsignedInteger capacity) {
			if (capacity > Capacity) {
				reallocate_and_copy(capacity);
			}
		}

		/**
		 * This function will reallocate the whole buffer if the size isn't exactly the size passed as parameters.
		 * @param size the new size of the HeapArray.
		 */
		void resize(const UnsignedInteger size)
		{
			if (size != Capacity)
			{
				reallocate_and_copy(size);
			}
			Count = Capacity;
		}

		/**
		 * The redimension function will change the count number to the specify size and will reallocate ONLY if the
		 * Capacity is not high enough to accommodate the new size.
		 * @param size the new count of the HeapArray
		 */
		void redimension(const UnsignedInteger size)
		{
			reallocate_if_necessary(size);
			Count = size;
		}

		// try_get(const UnsignedInteger index)
		// get(const UnsignedInteger index)
		// try_get(const UnsignedInteger index) const
		// get(const UnsignedInteger index) const
		// push_back
		// emplace_back
		// back
		// pop_back()
		// remove
		// swap_and_remove
		// clear
	public:
		// HeapArrayElement operator[](const UnsignedInteger index);
	private:
		void* data{nullptr};
		UnsignedInteger Count{0};
		UnsignedInteger Capacity{0};
		/**
		 * This is the size of one element as passed at the constructor of the array.
		 * The memory alignment, etc. Is for the user to handle before that.
		 */
		const UnsignedInteger DataSize;
	};
}
