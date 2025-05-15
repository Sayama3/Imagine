//
// Created by ianpo on 12/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/MemoryHelper.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"

namespace Imagine::Core {


	/**
	 * This heap array is a dynamically resizable array on the heap that handle allocation and deallocation of memory
	 * for a data whose type is fixed but only known at runtime. Example may include, a type created by a scripting engine
	 * whose type and therefore size is only known at runtime.
	 */
	template<typename UnsignedInteger = uint32_t>
	class RawHeapArray {
	private:

		void* GetPtr(const UnsignedInteger index) {
			return reinterpret_cast<uint8_t*>(data) + (DataSize * index);
		}

		const void* GetPtr(const UnsignedInteger index) const {
			return reinterpret_cast<uint8_t*>(data) + (DataSize * index);
		}

		UnsignedInteger GetByteCapacity() const { return Capacity * DataSize; }
		UnsignedInteger GetByteCount() const { return Count * DataSize; }

	public:
		RawHeapArray() noexcept {}
		/**
		 * Construct the RawHeapArray and set the data size that will be used for the rest of it's lifetime.
		 * @param dataSize The size of the component the array will use. Can only be set once.
		 */
		explicit RawHeapArray(const UnsignedInteger dataSize) noexcept : DataSize(dataSize) {}
		RawHeapArray(const UnsignedInteger dataSize, const UnsignedInteger capacity) : DataSize(dataSize) { reserve(capacity); }
		~RawHeapArray() {
			free(data);
			data = nullptr;
			Count = 0;
			Capacity = 0;
		}

		RawHeapArray(RawHeapArray&& other) noexcept {swap(other);}
		RawHeapArray& operator=(RawHeapArray&& other) noexcept {swap(other); return *this;}
		void swap(RawHeapArray& other) noexcept {
			std::swap(data, other.data);
			std::swap(Count, other.Count);
			std::swap(Capacity, other.Capacity);
			std::swap(DataSize, other.DataSize);
		}
	private:
		void c_swap_elements(const UnsignedInteger id1, const UnsignedInteger id2)
		{
			MGN_CORE_ASSERT(id1 < Capacity, "THe index {} is out of bound.", id1);
			MGN_CORE_ASSERT(id2 < Capacity, "THe index {} is out of bound.", id2);
			MemoryHelper::c_swap_memory(&reinterpret_cast<uint8_t*>(data)[id1*DataSize], &reinterpret_cast<uint8_t*>(data)[id2*DataSize], DataSize);
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

		template <typename... Args>
		void raw_push_back(const void* data, const uint64_t size, spdlog::format_string_t<Args...> fmt, Args &&...args) {
			const UnsignedInteger index = Count++;
			reallocate_if_necessary(Count);

			if (!data)
			{
				return;
			}

			MGN_CORE_CHECK(size == DataSize, fmt, std::forward<Args>(args)...);

			memcpy(GetPtr(index), data, std::min(size, static_cast<uint64_t>(DataSize)));

		}
	public:
		[[nodiscard]] UnsignedInteger is_valid() const {return data && DataSize>0;}
		[[nodiscard]] UnsignedInteger size() const {return Count;}
		[[nodiscard]] UnsignedInteger capacity() const {return Capacity;}
		[[nodiscard]] bool empty() const {return Count == 0;}

		/**
		 * Will reallocate the array if the desired capacity exceeds the one we have.
		 * @param capacity The new capacity of the array
		 */
		void reserve(const UnsignedInteger capacity) {
			if (capacity > Capacity) {
				reallocate_and_copy(capacity);
			}
		}

		/**
		 * QOL Function made to be able to ensure the array will be able to hold some additional_capacity without reallocating.
		 * The function will simply call the reserve function with size() + 'additional_capacity'.
		 * It's just a function I wish I add in the std::vector, so I added it.
		 * @param additional_capacity The number of elements we might add
		 */
		void prepare(const UnsignedInteger additional_capacity) {
			const UnsignedInteger capacity = Count + additional_capacity;
			reserve(capacity);
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

		UnsignedInteger get_data_size() const {
			return DataSize;
		}

		void* get(const UnsignedInteger index) {
#ifdef MGN_DEBUG
			MGN_ASSERT(data, "The buffer is not allocated yet.");
			MGN_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
			MGN_ASSERT(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
			return GetPtr(index);
		}

		const void* get(const UnsignedInteger index) const {
#ifdef MGN_DEBUG
			MGN_ASSERT(data, "The buffer is not allocated yet.");
			MGN_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
			MGN_ASSERT(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
			return GetPtr(index);
		}

		void* try_get(const UnsignedInteger index) {
			return ((!data) || index >= Count) ? nullptr : GetPtr(index);
		}

		const void* try_get(const UnsignedInteger index) const {
			return ((!data) || index >= Count) ? nullptr : GetPtr(index);
		}

		Buffer get_buffer(const UnsignedInteger index) const {
#ifdef MGN_DEBUG
			MGN_ASSERT(data, "The buffer is not allocated yet.");
			MGN_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
			MGN_ASSERT(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
			Buffer buff{ DataSize };
			memcpy(buff.Get(), GetPtr(index), DataSize);
			return std::move(buff);
		}

		Buffer try_get_buffer(const UnsignedInteger index) const {
			if (!data || index >= Count) return Buffer{};

			Buffer buff{ DataSize };
			memcpy(buff.Get(), GetPtr(index), DataSize);
			return std::move(buff);
		}

		BufferView get_view(const UnsignedInteger index) {
#ifdef MGN_DEBUG
			MGN_ASSERT(data, "The buffer is not allocated yet.");
			MGN_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
			MGN_ASSERT(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
			return {GetPtr(index), 0, DataSize};
		}

		BufferView try_get_view(const UnsignedInteger index) {
			if (!data || index >= Count) return BufferView{};
			return {GetPtr(index), 0, DataSize};
		}

		ConstBufferView get_const_view(const UnsignedInteger index) const {
#ifdef MGN_DEBUG
			MGN_ASSERT(data, "The buffer is not allocated yet.");
			MGN_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
			MGN_ASSERT(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
			return {GetPtr(index), 0, DataSize};
		}

		ConstBufferView try_get_const_view(const UnsignedInteger index) const {
			if (!data || index >= Count) return ConstBufferView{};
			return {GetPtr(index), 0, DataSize};
		}

		void push_back(const BufferView& view) {
			raw_push_back(view.Get(), view.Size(), "The BufferView sized '{}' given in parameter is not the same size as the DataSize '{}'. We will copy what we can.", view.Size(), DataSize);
		}

		void push_back(const Buffer& buffer) {
			raw_push_back(buffer.Get(), buffer.Size(), "The Buffer sized '{}' given in parameter is not the same size as the DataSize '{}'. We will copy what we can.", buffer.Size(), DataSize);
		}

		void push_back(const void* ptr, const uint64_t size) {
			raw_push_back(ptr, size, "The pointer sized '{}' given in parameter is not the same size as the DataSize '{}'. We will copy what we can.", size, DataSize);
		}

		template<typename T>
		void push_back(const T& data) {
			raw_push_back(&data, sizeof(T), "The Type '{}' sized '{}' given in parameter is not the same size as the DataSize '{}'. We will copy what we can.", typeid(T).name(), sizeof(T), DataSize);
		}

		void emplace_back() {
			const UnsignedInteger index = Count++;
			reallocate_if_necessary(Count);
		}

		void* back() {
			return GetPtr(Count - 1);
		}

		const void* back() const {
			return GetPtr(Count - 1);
		}

		void pop_back() {
			Count = Count == 0 ? 0 : Count - 1;
		}

		/**
		* This function DOESN'T call the destructor of the removed type. Call it BEFORE you call this function if you need to.
		*/
		void remove(const UnsignedInteger index)
		{
			if (index >= Count) return;
			const UnsignedInteger element_to_move = Count - index;
			for (int i = index + 1; i < element_to_move; ++i)
			{
				const auto previous = (i-1)*DataSize;
				const auto current = i * DataSize;
				memcpy(reinterpret_cast<uint8_t*>(data) + previous, reinterpret_cast<uint8_t*>(data) + current, DataSize);
			}
			--Count;
		}

		/**
		* This function DOESN'T call the destructor of the removed type. Call it BEFORE you call this function if you need to.
		* But the data will remain at index Count until further operation but not recommended to do it after.
		*/
		void swap_and_remove(const UnsignedInteger index)
		{
			c_swap_elements(index, Count - 1);
			--Count;
		}

		void clear() {
			Count = 0;
		}
	public:
		// RawHeapArrayElement operator[](const UnsignedInteger index);
	private:
		void* data{nullptr};
		UnsignedInteger Count{0};
		UnsignedInteger Capacity{0};
		/**
		 * This is the size of one element as passed at the constructor of the array.
		 * The memory alignment, etc. Is for the user to handle before that.
		 */
		UnsignedInteger DataSize{0};
	};
}
