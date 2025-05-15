//
// Created by Iannis on 10/05/2025.
//

#pragma once

#include "Macros.hpp"
#include "MemoryHelper.hpp"

namespace Imagine::Core
{
    /**
     * This heap array is a dynamically resizable array on the heap that handle allocation and deallocation of memory
     * WITHOUT calling any constructor. This is a DataStructure specifically made for that so if you need to call constructor
     * and destructor on the data you use before you do any operation. Be sure to do so.
     * Else, use the std::vector that do just that.
     *
     * @tparam T Element Type used in the HeapArray
     */
    template<typename T, typename UnsignedInteger = uint64_t>
    class HeapArray {
    public:
        HeapArray() noexcept {}
        explicit HeapArray(const UnsignedInteger capacity) {
            reserve(capacity);
        }
        ~HeapArray()
        {
            free(data);
            data = nullptr;
            Count = 0;
            Capacity = 0;
        }

        HeapArray(const HeapArray<T,UnsignedInteger>& other) : Count{other.Count}, Capacity{other.Capacity} {
            data = reinterpret_cast<T*>(malloc(sizeof(T) * Capacity));
            if (!other.data) return;
            memcpy(data, other.data, Count * sizeof(T));
        }

        HeapArray& operator=(const HeapArray<T,UnsignedInteger>& other) {
            if (other.Capacity != Capacity)
            {
                free(data);
                Capacity = other.Capacity;
                data = malloc(sizeof(T)*Capacity);
            }
            Count = other.Count;
            if (other.data) {
                memcpy(data, other.data, Count * sizeof(T));
            }
            return *this;
        }

        HeapArray(HeapArray<T,UnsignedInteger>&& other) noexcept {swap(other);}
        HeapArray& operator=(HeapArray<T,UnsignedInteger>&& other) noexcept {swap(other); return *this;}

        void swap(HeapArray<T,UnsignedInteger>& other) noexcept {
            std::swap(data, other.data);
            std::swap(Count, other.Count);
            std::swap(Capacity, other.Capacity);
        }
    private:
        void c_swap_elements(const UnsignedInteger id1, const UnsignedInteger id2)
        {
            MGN_CORE_ASSERT(id1 < Capacity, "THe index {} is out of bound.", id1);
            MGN_CORE_ASSERT(id2 < Capacity, "THe index {} is out of bound.", id2);
            MemoryHelper::c_swap_memory(&data[id1], &data[id2]);
        }
        void reallocate_and_copy(const UnsignedInteger size)
        {
            T* tmpBuffer = reinterpret_cast<T*>(malloc(sizeof(T) * size));
            if (data)
            {
                memcpy(tmpBuffer, data, std::min(Count, size)*sizeof(T));
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
    public:
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

        [[nodiscard]] T* try_get(const UnsignedInteger index)
        {
            if (index >= Count) return nullptr;
            return data[index];
        }

        [[nodiscard]] T& get(const UnsignedInteger index)
        {
#ifdef MGN_DEBUG
            MGN_ASSERT(data, "The buffer is not allocated yet.");
            MGN_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
            MGN_ASSERT(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
            return data[index];
        }

        [[nodiscard]] const T* try_get(const UnsignedInteger index) const
        {
            if (index >= Count) return nullptr;
            return data[index];
        }

        [[nodiscard]] const T& get(const UnsignedInteger index) const
        {
#ifdef MGN_DEBUG
            MGN_ASSERT(data, "The buffer is not allocated yet.");
            MGN_CORE_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
            MGN_CORE_CHECK(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
            return data[index];
        }

        void push_back(const T& copy)
        {
            const UnsignedInteger index = Count++;
            reallocate_if_necessary(Count);
            data[index] = copy;
        }

        void emplace_back()
        {
            const UnsignedInteger index = Count++;
            reallocate_if_necessary(Count);
        }

        T& back() {
            return get(Count - 1);
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
            for (int i = index+1; i < element_to_move; ++i)
            {
                memcpy(&data[i-1], &data[i], sizeof(T));
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
        T& operator[](const UnsignedInteger index) {
            return get(index);
        }
        const T& operator[](const UnsignedInteger index) const {
            return get(index);
        }
    private:
        T* data{nullptr};
        UnsignedInteger Count{0};
        UnsignedInteger Capacity{0};
    };

}
