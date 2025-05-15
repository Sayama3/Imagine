//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "HeapArray.hpp"
#include "MemoryHelper.hpp"

namespace Imagine::Core
{
    /**
     * This is a sparse set maybe using HeapArray that suppose the ids will come incrementally from 0 (and may get re-used)
     * as it's based on this to re-allocate the array of ids inside. Adding the element ID 18000 WILL reallocate the whole
     * data structure to be able to handle 18000 elements at least (Some overhead included). Be careful when using it.
     *
     * @tparam T The Type of data stored in the sparse set
     * @tparam UnsignedInteger The type of integer used in the sparse set
     */
    template<typename T, typename UnsignedInteger = uint32_t>
    class SparseSet {
        static_assert(std::is_unsigned_v<UnsignedInteger> == true);
    public:
        static inline constexpr UnsignedInteger c_OverheadResize = 64;
    private:
        bool RawCreate(const UnsignedInteger id) {

            if (Exist(id)) return false;

            // As elements are contiguous in memory, add the dense array count is the next element index.
            const UnsignedInteger index = dense.size();

            // Check that each of the three arrays has enough space for the element insertion.
            if (dense.capacity() <= index) {
                dense.reserve(index + c_OverheadResize);
                elements.reserve(index + c_OverheadResize);
            }
            if (sparse.size() <= id) {
                sparse.resize(id + c_OverheadResize);
            }

            // Add the elements in the arrays
            dense.emplace_back();
            elements.emplace_back();

            // Set the elements two-way connection
            sparse[id] = index;
            dense[index] = id;

            return true;
        }
    public:
        SparseSet() {sparse.resize(256);dense.reserve(256);elements.reserve(256);}
        explicit SparseSet(const UnsignedInteger capacity) {sparse.resize(capacity);dense.reserve(capacity);elements.reserve(capacity);}

        virtual ~SparseSet() {
            for (int i = 0; i < elements.size(); ++i) {
                elements[i].~T();
            }
            elements.clear();
            dense.clear();
            sparse.clear();
        }

        [[nodiscard]] bool Exist(const UnsignedInteger id) const {
            if (id >= sparse.size()) return false;
            const UnsignedInteger index = sparse[id];
            if (index >= dense.size()) return false;
            return dense[index] == id;
        }

        virtual bool Create(const UnsignedInteger id) {
            if (!RawCreate(id)) return false;

            // Initialise the data.
            new (&elements[sparse[id]]) T ();

            return true;
        }

        virtual bool Create(const UnsignedInteger id, const T& data) {
            if (!RawCreate(id)) return false;

            // Initialise the data.
            new (&elements[sparse[id]]) T(data);

            return true;
        }

        virtual void Remove(const UnsignedInteger id)
        {
            // Check the id is used. Cause no point doing work for naught.
            if (!Exist(id)) return;

            UnsignedInteger index = sparse[id];

            const UnsignedInteger last_index = dense.size() - 1;
            // Only doing the expensive bit of work if we need a swap.
            if (index != last_index)
            {
                // We are not the last one, so to not make any hole, we do a swap and delete.
                const UnsignedInteger swapIndex = last_index;
                const UnsignedInteger swapId = dense[swapIndex];

                MemoryHelper::c_swap_memory<T>(elements[index], elements[swapIndex]);
                MemoryHelper::c_swap_memory<UnsignedInteger>(dense[index], dense[swapIndex]);
                MemoryHelper::c_swap_memory<UnsignedInteger>(sparse[id], sparse[swapId]);

                index = swapIndex;
            }

            // Calling destructor cause the type T is not in use anymore.
            elements[index].~T();

            // Removing the index from existing.
            elements.pop_back();
            dense.pop_back();

            // Safely setting the sparse to -1 just in case anything happens.
            // But is probably useless and might be worth removing to remove a useless CPU instruction.
            sparse[id] = -1;
        }

        virtual void Clear() {
            for (int i = dense.size() - 1; i >= 0; --i) {
                Remove(dense[i]);
            }
        }

        [[nodiscard]] virtual T* TryGet(const UnsignedInteger id) {
            if (!Exist(id)) return nullptr;
            return &elements[sparse[id]];
        }

        [[nodiscard]] virtual T& Get(const UnsignedInteger id) {
#ifdef MGN_DEBUG
            MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif
            return elements[sparse[id]];
        }
        [[nodiscard]] virtual const T* TryGet(const UnsignedInteger id) const {
            if (!Exist(id)) return nullptr;
            return &elements[sparse[id]];
        }

        [[nodiscard]] virtual const T& Get(const UnsignedInteger id) const {
#ifdef MGN_DEBUG
            MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif
            return elements[sparse[id]];
        }

        [[nodiscard]] UnsignedInteger Count() const {
            return dense.size();
        }

        [[nodiscard]] UnsignedInteger Capacity() const {
            return dense.capacity();
        }

    protected:
        HeapArray<UnsignedInteger, UnsignedInteger> sparse{};
        HeapArray<UnsignedInteger, UnsignedInteger> dense{};
        HeapArray<T, UnsignedInteger> elements{};
    };


    template<typename T, typename UnsignedInteger = uint32_t>
    class AutoIdSparseSet : public SparseSet<T, UnsignedInteger> {
    public:
        AutoIdSparseSet() : SparseSet<T, UnsignedInteger>() {FreeList.reserve(256); FreeList.reserve(256);}
        explicit AutoIdSparseSet(const UnsignedInteger capacity) : SparseSet<T, UnsignedInteger>(capacity) {FreeList.reserve(capacity); FreeList.reserve(capacity);}
        virtual ~AutoIdSparseSet() override = default;
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

        void EnsureFreelistContinuityOnCreate(const UnsignedInteger id) {
            if (id >= IDs) {
                const UnsignedInteger nextIDs = (id+1);
                const UnsignedInteger numberIdToAddInFreeList = nextIDs - IDs;
                FreeList.reserve(FreeList.size() + numberIdToAddInFreeList + SparseSet<T, UnsignedInteger>::c_OverheadResize);
                FreeList.redimension(FreeList.size() + numberIdToAddInFreeList);
                for (int i = IDs; i < id; ++i) {
                    FreeList.push_back(i);
                }
                IDs = nextIDs;
            } else if (!FreeList.empty()) {
                const UnsignedInteger* begin = &FreeList[0];
                const UnsignedInteger* end = &FreeList.back() + 1;

                const UnsignedInteger* idxPtr = std::find(begin, end, id);
                if (idxPtr != end) {
                    FreeList.swap_and_remove(idxPtr - begin);
                }
            }
        }
    public:
        static constexpr UnsignedInteger NullId{std::numeric_limits<UnsignedInteger>::max()};
    public:
        virtual UnsignedInteger Create() {
            const UnsignedInteger id = CreateID();
            const bool result = SparseSet<T, UnsignedInteger>::Create(id);
            return result ? id : NullId;
        }

        virtual UnsignedInteger Create(const T& data) {
            const UnsignedInteger id = CreateID();
            const bool result = SparseSet<T, UnsignedInteger>::Create(id, data);
            return result ? id : NullId;
        }

        virtual bool Create(const UnsignedInteger id) override {
            if (id == NullId) return false;
            EnsureFreelistContinuityOnCreate(id);
            return SparseSet<T, UnsignedInteger>::Create(id);
        }

        virtual bool Create(const UnsignedInteger id, const T& data) override {
            if (id == NullId) return false;
            EnsureFreelistContinuityOnCreate(id);
            return SparseSet<T, UnsignedInteger>::Create(id, data);
        }

        virtual void Remove(const UnsignedInteger id) override {
            if (id == NullId) return;
            if (!SparseSet<T, UnsignedInteger>::Exist(id)) {
                return;
            }
            FreeList.push_back(id);
            SparseSet<T, UnsignedInteger>::Remove(id);
        }

        virtual void Clear() override {
            for (int i = SparseSet<T, UnsignedInteger>::dense.size() - 1; i >= 0; --i) {
                const auto id = SparseSet<T, UnsignedInteger>::dense[i];
                FreeList.push_back(id);
                SparseSet<T, UnsignedInteger>::Remove(id);
            }
        }
    protected:
        HeapArray<UnsignedInteger> FreeList{SparseSet<T, UnsignedInteger>::c_OverheadResize};
        UnsignedInteger IDs{0};
    };
}
