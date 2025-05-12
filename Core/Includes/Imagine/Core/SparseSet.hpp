//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "HeapArray.hpp"
#include "MemoryHelper.hpp"

namespace Imagine::Core
{

    template<typename T, typename UnsignedInteger = uint32_t>
    class SparseSet {
        static_assert(std::is_unsigned_v<UnsignedInteger> == true);
    private:
        static inline constexpr UnsignedInteger c_OverheadResize = 64;
    private:
        UnsignedInteger CreateID() {
            UnsignedInteger id;
            if (!FreeList.empty()) {
                id = FreeList.back();
                FreeList.pop_back();
            } else {
                id = IDs++;
            }
            return id;
        } // This way, std::numeric_limits<UnsignedInteger>::max() is a null id.
    public:
        SparseSet() {sparse.resize(256);dense.resize(256);elements.resize(256);FreeList.reserve(256);}
        explicit SparseSet(const UnsignedInteger capacity) {sparse.resize(capacity);dense.resize(capacity);elements.resize(capacity);FreeList.reserve(256);}

        ~SparseSet() {
            for (int i = 0; i < Count; ++i) {
                elements[i].~T();
            }
        }

        UnsignedInteger Create()
        {
            const UnsignedInteger id = CreateID();
            const UnsignedInteger index = Count++;

            if (dense.size() < index) {
                dense.resize(index + c_OverheadResize);
                elements.resize(index + c_OverheadResize);
            }
            if (sparse.size() < id) {
                sparse.resize(id + c_OverheadResize);
            }

            sparse[id] = index;
            dense[index] = id;

            new (&elements[index]) T();

            return id;
        }

        void Remove(const UnsignedInteger id)
        {
            UnsignedInteger index = sparse[id];
            if (index != Count - 1)
            {
                // We are not the last one, so to not make any hole, we do a swap and delete.
                const UnsignedInteger swapIndex = Count - 1;
                const UnsignedInteger swapId = dense[swapIndex];

                MemoryHelper::c_swap_memory<T>(elements[index], elements[swapIndex]);
                MemoryHelper::c_swap_memory<UnsignedInteger>(dense[index], dense[swapIndex]);
                MemoryHelper::c_swap_memory<UnsignedInteger>(sparse[id], sparse[swapId]);

                index = swapIndex;
            }
            elements[index].~T();
            dense[index] = -1;
            sparse[id] = -1;
            --Count;
            FreeList.push_back(id);
        }
    private:
        HeapArray<UnsignedInteger> FreeList{c_OverheadResize};
        HeapArray<UnsignedInteger> sparse{};
        HeapArray<UnsignedInteger> dense{};
        HeapArray<T> elements{};
        UnsignedInteger IDs{0};
        UnsignedInteger Count{0};
    };

}
