//
// Created by Iannis on 10/05/2025.
//

#pragma once

#include "Macros.hpp"

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
    template<typename T>
    class HeapArray {
    public:
        HeapArray() {}
        ~HeapArray()
        {
            free(data);
        }
    private:
        template<typename Type>
        inline static void c_swap_memory(Type* id1, Type* id2)
        {
            //TODO: Optimize ?
            Type* tmp = malloc(sizeof(Type));
            memcpy(tmp, id1, sizeof(Type));
            memcpy(id1, id2, sizeof(Type));
            memcpy(id2, tmp, sizeof(Type));
            free(tmp);
        }

        inline void c_swap_elements(const uint64_t id1, const uint64_t id2)
        {
            MGN_CORE_ASSERT(id1 < Capacity, "THe index {} is out of bound.", id1);
            MGN_CORE_ASSERT(id2 < Capacity, "THe index {} is out of bound.", id2);
            c_swap_memory(&data[id1], &data[id2]);
        }

        void reallocate_and_copy(const uint64_t size)
        {
            T* tmpBuffer = malloc(sizeof(T)*size);
            if (data)
            {
                memcpy(tmpBuffer, data, std::min(Count, size)*sizeof(T));
            }
            std::swap(data, tmpBuffer);
            free(tmpBuffer);
            Capacity = size;
        }
        void reallocate_if_necessary(const uint64_t minimumCapacity)
        {
            if (Capacity < minimumCapacity)
            {
                reallocate_and_copy(minimumCapacity);
            }
        }
    public:
        [nodiscard] uint64_t size() const {return Count;}
        [nodiscard] uint64_t capacity() const {return Capacity;}
    public:
        /**
         * This function will reallocate the whole buffer if the size isn't exactly the size passed as parameters.
         * @param size the new size of the HeapArray.
         */
        void resize(const uint64_t size)
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
        void redimension(const uint64_t size)
        {
            reallocate_if_necessary(size);
            Count = size;
        }

        [nodiscard] T* try_get(const uint64_t index)
        {
            if (index >= Count) return nullptr;
            return data[index];
        }

        [nodiscard] T& get(const uint64_t index)
        {
#ifdef MGN_DEBUG
            MGN_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
            MGN_ASSERT(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
            return data[index];
        }

        [nodiscard] const T* try_get(const uint64_t index) const
        {
            if (index >= Count) return nullptr;
            return data[index];
        }

        [nodiscard] const T& get(const uint64_t index) const
        {
#ifdef MGN_DEBUG
            MGN_CORE_ASSERT(index < Capacity, "The index ({}) is not in the allocated ({}) bounds.", index, Capacity);
            MGN_CORE_CHECK(index < Count, "The index ({}) is not in the Count ({}) bounds.", index, Count);
#endif
            return data[index];
        }

        T& push_back(const T& copy)
        {
            const uint64_t index = Count++;
            reallocate_if_necessary(Count);
            return data[index];
        }

        T& emplace_back()
        {
            const uint64_t index = Count++;
            reallocate_if_necessary(Count);
            return data[index];
        }

        /**
        * This function DOESN'T call the destructor of the removed type. Call it BEFORE you call this function if you need to.
        */
        void remove(const uint64_t index)
        {
            if (index >= Count) return;
            const uint64_t element_to_move = Count - index;
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
        void swap_and_remove(const uint64_t index)
        {
            c_swap_elements(index, Count - 1);
            std::swap(data[index], data[Count-1]);
            --Count;
        }
    private:
        T* data{nullptr};
        uint64_t Count{0};
        uint64_t Capacity{0};
    };

}
