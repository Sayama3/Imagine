//
// Created by ianpo on 12/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/RawHeapArray.hpp"

namespace Imagine::Core {

	template<typename UnsignedInteger = uint32_t>
	class RawSparseSet {
		static_assert(std::is_unsigned_v<UnsignedInteger> == true);
	public:
		static inline constexpr UnsignedInteger c_OverheadResize = 64;
	public:
		RawSparseSet(const UnsignedInteger dataSize) : sparse(256), dense(256), elements(dataSize, 256) { sparse.redimension(256);}
		RawSparseSet(const UnsignedInteger dataSize, const UnsignedInteger capacity) : sparse(capacity), dense(capacity), elements(dataSize, capacity) { sparse.redimension(capacity); }
		virtual ~RawSparseSet() {
            if (destructor)
            {
                const UnsignedInteger dataSize = GetDataSize();
                for (int i = 0; i < elements.size(); ++i) {
                    destructor(elements.get(i), dataSize);
                }
            }
			elements.clear();
		}
	public:
        /// <summary>
        /// Function allowing to pass a function that will be used to initialize the data passed in parameter.
        /// </summary>
        /// <param name="constructor">The constructor parameter taking both the data pointer and it's byte size.</param>
        void SetConstructor(void(*constructor)(void*, UnsignedInteger)) {
            this->constructor = constructor;
        }

        /// <summary>
        /// Function allowing to pass a function that will be used to initialize the data passed in parameter.
        /// </summary>
        /// <param name="constructor">The constructor parameter taking both the data pointer and it's byte size.</param>
        void SetCopyConstructor(void(*constructor)(void*, UnsignedInteger, BufferView view)) {
            this->copy_constructor = constructor;
        }

        /// <summary>
        /// Function allowing to pass a function that will be used to destroy the data passed in parameter.
        /// </summary>
        /// <param name="destructor">The destructor parameter taking both the data pointer and it's byte size.</param>
        void SetDestructor(void(*destructor)(void*, UnsignedInteger)) {
            this->destructor = destructor;
        }
	public:
		UnsignedInteger GetDataSize() const { return elements.get_data_size(); }

        [[nodiscard]] bool Exist(const UnsignedInteger id) const {
            if (id >= sparse.size()) return false;
            const UnsignedInteger index = sparse[id];
            if (index >= dense.size()) return false;
            return dense[index] == id;
        }

        virtual bool Create(const UnsignedInteger id) {
            if (!RawCreate(id)) return false;

            // Initialise the data.
            if (constructor) {
                constructor(elements.get(sparse[id]), GetDataSize());
            }

            return true;
        }

        virtual bool Create(const UnsignedInteger id, const BufferView& view) {
            if (!RawCreate(id)) return false;

            // Initialise the data.
            if (copy_constructor) {
                copy_constructor(elements.get(sparse[id]), GetDataSize(), view);
            } else if (constructor) {
                MGN_CORE_WARN("No copy constructor found. Using the default constructor.");
                constructor(elements.get(sparse[id]), GetDataSize());
            }

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

                MemoryHelper::c_swap_memory(elements.get(index), elements.get(swapIndex), GetDataSize());
                MemoryHelper::c_swap_memory<UnsignedInteger>(dense[index], dense[swapIndex]);
                MemoryHelper::c_swap_memory<UnsignedInteger>(sparse[id], sparse[swapId]);

                index = swapIndex;
            }

            // Calling destructor cause the type T is not in use anymore.
            if (destructor) {
                destructor(elements.get(index), GetDataSize());
            }

            // Removing the index from existing.
            elements.pop_back();
            dense.pop_back();

            // Safely setting the sparse to -1 just in case anything happens.
            // But is probably useless and might be worth removing to remove a useless CPU instruction.
            sparse[id] = -1;
        }

        [[nodiscard]] virtual void* TryGet(const UnsignedInteger id) {
            if (!Exist(id)) return nullptr;
            return elements.get(sparse[id]);
        }

        [[nodiscard]] virtual void* Get(const UnsignedInteger id) {
#ifdef MGN_DEBUG
            MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif
            return elements.get(sparse[id]);
        }
        [[nodiscard]] virtual const void* TryGet(const UnsignedInteger id) const {
            if (!Exist(id)) return nullptr;
            return elements.get(sparse[id]);
        }

        [[nodiscard]] virtual const void* Get(const UnsignedInteger id) const {
#ifdef MGN_DEBUG
            MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif
            return elements.get(sparse[id]);
        }

        [[nodiscard]] UnsignedInteger Count() const {
            return dense.size();
        }

        [[nodisard]] UnsignedInteger Capacity() const{
            return dense.capacity();
        }


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
		}
	private:
		HeapArray<UnsignedInteger, UnsignedInteger> sparse{};
		HeapArray<UnsignedInteger, UnsignedInteger> dense{};
		RawHeapArray<UnsignedInteger> elements{};
        void(*constructor)(void*, UnsignedInteger) = nullptr;
        void(*destructor)(void*, UnsignedInteger) = nullptr;
        void(*copy_constructor)(void*, UnsignedInteger, BufferView view) = nullptr;
	};

}
