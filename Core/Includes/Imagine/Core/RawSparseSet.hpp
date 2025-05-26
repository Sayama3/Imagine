//
// Created by ianpo on 12/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/RawHeapArray.hpp"
#include "Imagine/Core/HeapArray.hpp"

namespace Imagine::Core {

	/**
	 * This is a sparse set using HeapArray that suppose the ids will come incrementally from 0 (and may get re-used)
	 * as it's based on this to re-allocate the array of ids inside.
	 * Adding the element ID 18,000 WILL reallocate the whole
	 * data structure to be able to handle 18,000 elements at least (Some overhead included).
	 * Be careful when using it.
	 *
	 * The difference with SparseSet Data Structure is
	 * that it doesn't allocate a specific type but rather a fixed chunk of memory.
	 * It will initialize it if given the function pointer, but it can just copy memory if needed.
	 *
	 * The primary intent was to be able to create a runtime data type and store it continuously in memory.
	 * It works just fine for this purpose.
	 *
	 * If you want to use a certain Type, you can, even if it's less than ideal and the SparseSet does it better,
	 * but for tests purposes, I added some functionality that makes it reasonably possible.
	 * @tparam UnsignedInteger The type of integer used in the sparse set
	 */
	template<typename UnsignedInteger = uint32_t>
	class RawSparseSet {
		static_assert(std::is_unsigned_v<UnsignedInteger> == true);
	public:
		static inline constexpr UnsignedInteger c_OverheadResize = 64;
	public:
		template<typename T>
		inline static RawSparseSet Instantiate() {
			RawSparseSet sparseSet = RawSparseSet<UnsignedInteger>{sizeof(T)};

			sparseSet.SetConstructor([](void* data, const UnsignedInteger size){new(data) T();});
			sparseSet.SetDestructor([](void* data, const UnsignedInteger size){reinterpret_cast<T*>(data)->~T();});
			sparseSet.SetCopyConstructor([](void* data, const UnsignedInteger size, ConstBufferView view){new (data) T(view.As<T>());});

			return sparseSet;
		}

		template<typename T>
		inline static RawSparseSet Instantiate(const UnsignedInteger capacity) {
			RawSparseSet sparseSet = RawSparseSet<UnsignedInteger>{sizeof(T), capacity};

			sparseSet.SetConstructor([](void* data, const UnsignedInteger size){new(data) T();});
			sparseSet.SetDestructor([](void* data, const UnsignedInteger size){reinterpret_cast<T*>(data)->~T();});
			sparseSet.SetCopyConstructor([](void* data, const UnsignedInteger size, ConstBufferView view){new (data) T(view.As<T>());});

			return sparseSet;
		}
	public:
		RawSparseSet() noexcept {}
		explicit RawSparseSet(const UnsignedInteger dataSize) : sparse(256), dense(256), elements(dataSize, 256) { sparse.redimension(256);}
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

		RawSparseSet(const RawSparseSet& other) : sparse(other.sparse), dense(other.dense), elements(other.elements) {}
		RawSparseSet& operator=(const RawSparseSet& other) {
			sparse = other.sparse;
			dense = other.dense;
			elements = other.elements;
			return *this;
		}

		RawSparseSet(RawSparseSet&& other) noexcept : sparse(), dense(), elements() {swap(other);}
		RawSparseSet& operator=(RawSparseSet&& other) noexcept {swap(other); return *this;}

		void swap(RawSparseSet& other) noexcept {
			sparse.swap(other.sparse);
			dense.swap(other.dense);
			elements.swap(other.elements);
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
        /// <param name="constructor">The constructor parameter taking both the data pointer, its byte size and a memory view to some data that should be used as Input.</param>
        void SetCopyConstructor(void(*constructor)(void*, UnsignedInteger, ConstBufferView view)) {
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

		/**
		 * Allocate a bloc of memory and initialize it if the function as been passed down.
		 * Leave it untouched if no function is found.
		 * @param id The ID of the data we create
		 * @return Whether we successfully created the data.
		 */
		virtual bool Create(const UnsignedInteger id) {
            if (!RawCreate(id)) return false;

            // Initialize the data.
            if (constructor) {
                constructor(elements.get(sparse[id]), GetDataSize());
            }

            return true;
        }

		/**
		 * Allocate a bloc of memory and initialize it with the memory view if the function as been passed down.
		 * Copy a maximum of memory if no function is found.
		 * It won't call the default constructor function either, just copying the memory where it should be.
		 * @param id The ID of the data we create
		 * @param view A view of some arbitrary data in memory.
		 * @return Whether we successfully allocated the data.
		 */
		virtual bool Create(const UnsignedInteger id, const ConstBufferView& view) {
            if (!RawCreate(id)) return false;

            // Initialize the data.
            if (copy_constructor) {
                copy_constructor(elements.get(sparse[id]), GetDataSize(), view);
            } else {
	            memcpy(elements.get(sparse[id]), view.Get(), std::min(view.Size(), static_cast<uint64_t>(GetDataSize())));
            }

            return true;
        }


		/**
		 * Allocate a bloc of memory and initialize it with the memory view if the function as been passed down.
		 * Copy a maximum of memory if no function is found.
		 * It won't call the default constructor function either, just copying the memory where it should be.
		 * @param id The ID of the data we create
		 * @param buffer A buffer of some arbitrary data in memory.
		 * @return Whether we successfully allocated the data.
		 */
        virtual bool Create(const UnsignedInteger id, const Buffer& buffer) {
            if (!RawCreate(id)) return false;

            // Initialize the data.
            if (copy_constructor) {
                copy_constructor(elements.get(sparse[id]), GetDataSize(), ConstBufferView::MakeSlice(&buffer,0,buffer.Size()));
            } else {
	            memcpy(elements.get(sparse[id]), buffer.Get(), std::min(buffer.Size(), static_cast<uint64_t>(GetDataSize())));
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


		virtual void Clear() {
			for (int64_t i = dense.size() - 1; i >= 0; --i) {
				const auto id = dense.get(i);
				Remove(id);
			}
		}

		virtual void Reserve(const UnsignedInteger capacity) {
			sparse.reserve(capacity);
			dense.reserve(capacity);
			elements.reserve(capacity);
		}

		virtual void Prepare(const UnsignedInteger additional_capacity) {
			sparse.reserve(dense.size() + additional_capacity);
			dense.prepare(additional_capacity);
			elements.prepare(additional_capacity);
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

		[[nodiscard]] virtual Buffer GetBuffer(const UnsignedInteger id) const {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif

			return elements.get_buffer(sparse[id]);
		}

		[[nodiscard]] virtual Buffer TryGetBuffer(const UnsignedInteger id) const {
            if (!Exist(id)) return Buffer{};

			return elements.get_buffer(sparse[id]);
		}

		[[nodiscard]] virtual BufferView GetView(const UnsignedInteger id) {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif

			return elements.get_view(sparse[id]);
		}

		[[nodiscard]] virtual BufferView TryGetView(const UnsignedInteger id) {
            if (!Exist(id)) return BufferView{};

			return elements.get_view(sparse[id]);
		}

		[[nodiscard]] virtual ConstBufferView GetConstView(const UnsignedInteger id) const {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif

			return elements.get_const_view(sparse[id]);
		}

		[[nodiscard]] virtual ConstBufferView TryGetConstView(const UnsignedInteger id) const {
            if (!Exist(id)) return ConstBufferView{};

			return elements.get_const_view(sparse[id]);
		}

        [[nodiscard]] UnsignedInteger Count() const {
            return dense.size();
        }

        [[nodiscard]] UnsignedInteger Capacity() const {
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
			return true;
		}
	protected:
		HeapArray<UnsignedInteger, UnsignedInteger> sparse{};
		HeapArray<UnsignedInteger, UnsignedInteger> dense{};
		RawHeapArray<UnsignedInteger> elements{};
	private:
        void(*constructor)(void*, UnsignedInteger) = nullptr;
        void(*destructor)(void*, UnsignedInteger) = nullptr;
        void(*copy_constructor)(void*, UnsignedInteger, ConstBufferView view) = nullptr;
	};

	/**
	 * A Raw Sparse Set where we manage the IDs inside the Data Structure using a FreeList and incremental IDs.
	 * @tparam UnsignedInteger The unsigned integer used to count, measure, etc. wherever it's necessary.
	 */
	template<typename UnsignedInteger = uint32_t>
	class AutoIdRawSparseSet : public RawSparseSet<UnsignedInteger> {
	public:

		template<typename T>
		inline static AutoIdRawSparseSet Instantiate() {
			AutoIdRawSparseSet sparseSet = AutoIdRawSparseSet<UnsignedInteger>{sizeof(T)};

			sparseSet.SetConstructor([](void* data, const UnsignedInteger size){new(data) T();});
			sparseSet.SetDestructor([](void* data, const UnsignedInteger size){reinterpret_cast<T*>(data)->~T();});
			sparseSet.SetCopyConstructor([](void* data, const UnsignedInteger size, ConstBufferView view){new (data) T(view.As<T>());});

			return sparseSet;
		}

		template<typename T>
		inline static AutoIdRawSparseSet Instantiate(const UnsignedInteger capacity) {
			AutoIdRawSparseSet sparseSet = AutoIdRawSparseSet<UnsignedInteger>{sizeof(T), capacity};

			sparseSet.SetConstructor([](void* data, const UnsignedInteger size){new(data) T();});
			sparseSet.SetDestructor([](void* data, const UnsignedInteger size){reinterpret_cast<T*>(data)->~T();});
			sparseSet.SetCopyConstructor([](void* data, const UnsignedInteger size, ConstBufferView view){new (data) T(view.As<T>());});

			return sparseSet;
		}
	public:
        AutoIdRawSparseSet() : RawSparseSet<UnsignedInteger>() {}
        explicit AutoIdRawSparseSet(const UnsignedInteger dataSize) : RawSparseSet<UnsignedInteger>(dataSize) {FreeList.reserve(256); FreeList.reserve(256);}
        explicit AutoIdRawSparseSet(const UnsignedInteger dataSize, const UnsignedInteger capacity) : RawSparseSet<UnsignedInteger>(dataSize, capacity) {FreeList.reserve(capacity); FreeList.reserve(capacity);}
        virtual ~AutoIdRawSparseSet() override {
	        FreeList.clear();
        	IDs = 0;
        }
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
                FreeList.reserve(FreeList.size() + numberIdToAddInFreeList + RawSparseSet<UnsignedInteger>::c_OverheadResize);
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
		/**
		 * Allocate a bloc of memory and initialize it if the function as been passed down.
		 * Leave it untouched if no function is found.
		 * It will also create and assign an available ID. It will re-use the returned ID.
		 * @return The ID created if it was successful. NullId else.
		 */

        virtual UnsignedInteger Create() {
            const UnsignedInteger id = CreateID();
            const bool result = RawSparseSet<UnsignedInteger>::Create(id);
            return result ? id : NullId;
        }

		/**
		 * Allocate a bloc of memory and initialize it with the memory view if the function as been passed down.
		 * Copy a maximum of memory if no function is found.
		 * It won't call the default constructor function either, just copying the memory where it should be.
		 * It will also create and assign an available ID. It will re-use the returned ID.
		 * @param view A view of some arbitrary data in memory.
		 * @return The ID created if it was successful. NullId else.
		 */
        virtual UnsignedInteger Create(const ConstBufferView& view) {
            const UnsignedInteger id = CreateID();
            const bool result = RawSparseSet<UnsignedInteger>::Create(id, view);
            return result ? id : NullId;
        }

		/**
		 * Allocate a bloc of memory and initialize it with the memory view if the function as been passed down.
		 * Copy a maximum of memory if no function is found.
		 * It won't call the default constructor function either, just copying the memory where it should be.
		 * It will also create and assign an available ID. It will re-use the returned ID.
		 * @param buffer A buffer of some arbitrary data in memory.
		 * @return The ID created if it was successful. NullId else.
		 */
        virtual UnsignedInteger Create(const Buffer& buffer) {
            const UnsignedInteger id = CreateID();
            const bool result = RawSparseSet<UnsignedInteger>::Create(id, buffer);
            return result ? id : NullId;
        }

		/**
		 * Allocate a bloc of memory and initialize it if the function as been passed down.
		 * Leave it untouched if no function is found.
		 * @param id The ID of the data we create
		 * @return Whether we successfully created the data.
		 */
        virtual bool Create(const UnsignedInteger id) override {
            if (id == NullId) return false;
            EnsureFreelistContinuityOnCreate(id);
            return RawSparseSet<UnsignedInteger>::Create(id);
        }

		/**
		 * Allocate a bloc of memory and initialize it with the memory view if the function as been passed down.
		 * Copy a maximum of memory if no function is found.
		 * It won't call the default constructor function either, just copying the memory where it should be.
		 * @param id The ID of the data we create
		 * @param buffer A buffer of some arbitrary data in memory.
		 * @return Whether we successfully allocated the data.
		 */
        virtual bool Create(const UnsignedInteger id, const Buffer& buffer) override {
            if (id == NullId) return false;
            EnsureFreelistContinuityOnCreate(id);
            return RawSparseSet<UnsignedInteger>::Create(id, buffer);
        }

		/**
		 * Allocate a bloc of memory and initialize it with the memory view if the function as been passed down.
		 * Copy a maximum of memory if no function is found.
		 * It won't call the default constructor function either, just copying the memory where it should be.
		 * @param id The ID of the data we create
		 * @param view A view of some arbitrary data in memory.
		 * @return Whether we successfully allocated the data.
		 */
        virtual bool Create(const UnsignedInteger id, const ConstBufferView& view) override {
            if (id == NullId) return false;
            EnsureFreelistContinuityOnCreate(id);
            return RawSparseSet<UnsignedInteger>::Create(id, view);
        }

        virtual void Remove(const UnsignedInteger id) override {
            if (id == NullId) return;
            if (!RawSparseSet<UnsignedInteger>::Exist(id)) {
                return;
            }
            FreeList.push_back(id);
            RawSparseSet<UnsignedInteger>::Remove(id);
        }

        virtual void Clear() override {
            for (int i = RawSparseSet<UnsignedInteger>::dense.size() - 1; i >= 0; --i) {
                const auto id = RawSparseSet<UnsignedInteger>::dense[i];
                FreeList.push_back(id);
                RawSparseSet<UnsignedInteger>::Remove(id);
            }
        }
    protected:
        HeapArray<UnsignedInteger> FreeList{RawSparseSet<UnsignedInteger>::c_OverheadResize};
        UnsignedInteger IDs{0};
	};
}
