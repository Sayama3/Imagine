//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "HeapArray.hpp"

namespace Imagine::Core
{

    template<typename T, typename UnsignedInteger = uint32_t>
    class SparseSet {
        static_assert(std::is_unsigned_v<UnsignedInteger> == true);
    private:
        UnsignedInteger IDs;
        //TODO: Implement a freelist.
        UnsignedInteger CreateID() {return IDs++;} // This way, std::numeric_limits<UnsignedInteger>::max() is a null id.
    public:
        SparseSet() {sparse.resize(256);dense.resize(256);elements.resize(256);}
        explicit SparseSet(UnsignedInteger capacity) {sparse.resize(capacity);dense.resize(capacity);elements.resize(capacity);}

        UnsignedInteger Create()
        {
            const UnsignedInteger id = CreateID();
            const UnsignedInteger index = Count++;
            dense.emplace_back();
            elements.emplace_back();

            if (sparse.size() < id) sparse.resize(id+1); //TODO: Maybe resize like way more. And reuse ID
            sparse[id] = index;
            dense[index] = id;
            return id;
        }

        void Remove(const UnsignedInteger id)
        {
            UnsignedInteger index = sparse[id];
            if (index != dense.size() - 1)
            {
                // We are not the last one, so to not make any hole, we do a swap and delete.
                const UnsignedInteger swapIndex = dense.size() - 1;
                const UnsignedInteger swapId = dense[swapIndex];

                std::swap(elements[index], elements[swapIndex]);
                std::swap(dense[index], dense[swapIndex]);
                std::swap(sparse[id], sparse[swapId]);

                index = swapIndex;
            }
            sparse[id] = -1;
            elements[index].operator~();
            --Count;
        }
    private:
        HeapArray<UnsignedInteger> sparse; //TODO: change with an unordered map ?
        HeapArray<UnsignedInteger> dense;
        HeapArray<T> elements;
        UnsignedInteger Count;
    };

}
