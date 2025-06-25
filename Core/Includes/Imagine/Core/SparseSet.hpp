//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "HeapArray.hpp"
#include "MemoryHelper.hpp"

namespace Imagine::Core {
	/**
	 * This is a sparse set using HeapArray that suppose the ids will come incrementally from 0 (and may get re-used)
	 * as it's based on this to re-allocate the array of ids inside. Adding the element ID 18000 WILL reallocate the whole
	 * data structure to be able to handle 18000 elements at least (Some overhead included). Be careful when using it.
	 *
	 * @tparam T The Type of data stored in the sparse set
	 * @tparam UnsignedInteger The type of integer used in the sparse set
	 */
	template<typename T, typename UnsignedInteger = uint32_t, bool CallDestructorT = true>
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
		SparseSet() {
			sparse.resize(256);
			dense.reserve(256);
			elements.reserve(256);
		}
		explicit SparseSet(const UnsignedInteger capacity) {
			sparse.resize(capacity);
			dense.reserve(capacity);
			elements.reserve(capacity);
		}
		SparseSet(const SparseSet& o) {
			sparse = o.sparse;
			dense = o.dense;
			elements.redimension(dense.size());
			for (UnsignedInteger i = 0; i < dense.size(); ++i) {
				new (&elements[i]) T(o.elements[i]);
			}
		}
		SparseSet& operator=(const SparseSet& o) {
			if constexpr (CallDestructorT) {
				for (UnsignedInteger i = 0; i < elements.size(); ++i) {
					elements.get(i).~T();
				}
			}
			sparse = o.sparse;
			dense = o.dense;
			elements.redimension(dense.size());
			for (UnsignedInteger i = 0; i < dense.size(); ++i) {
				new (&elements[i]) T(o.elements[i]);
			}
			return *this;
		}
		SparseSet(SparseSet&&s) noexcept {swap(s);}
		SparseSet& operator=(SparseSet&& s) noexcept { swap(s); return *this;}
		void swap(SparseSet& s) noexcept {
			sparse.swap(sparse);
			dense.swap(dense);
			elements.swap(elements);
		}

		virtual ~SparseSet() {
			if constexpr( CallDestructorT) {
				for (int i = 0; i < elements.size(); ++i) {
					 elements[i].~T();
				}
			}
			elements.clear();
			dense.clear();
			sparse.clear();
		}

	public:
	public:
		class Iterator {
		public:
			Iterator() = default;
			~Iterator() = default;
			Iterator(const Iterator&) = default;
			Iterator& operator=(const Iterator&) = default;
			Iterator(SparseSet* sparseSet, UnsignedInteger index) : sparseSet(sparseSet), index(index) {}
		public:
			inline static Iterator FromID(SparseSet* sparseSet, UnsignedInteger id) {
				return Iterator {sparseSet, sparseSet->sparse[id]};
			}
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;
		public:
			Iterator operator++(int) {
				Iterator res{*this};
				++(*this);
				return res;
			}
			Iterator &operator++() {
				++index;
				return *this;
			}
			Iterator operator--(int) {
				Iterator res{*this};
				--(*this);
				return res;
			}
			Iterator &operator--() {
				--index;
				return *this;
			}

			reference operator*() { return GetElement(); }
			pointer operator->() { return &GetElement(); }

			const reference operator*() const { return GetElement(); }
			const pointer operator->() const { return &GetElement(); }

			bool operator==(const Iterator &o) const { return sparseSet == o.sparseSet && index == o.index; }
			bool operator!=(const Iterator &o) const { return !(*this == o); }
			auto operator<=>(const Iterator &o) const { return index <=> o.index; }
		public:
			UnsignedInteger GetID() const {return sparseSet->dense[index];}
			UnsignedInteger GetIndex() const {return index;}
			reference GetElement()  {return sparseSet->elements.get(index);}
			const reference GetElement() const  {return sparseSet->elements.get(index);}
		private:
			SparseSet* sparseSet{nullptr};
			UnsignedInteger index{0};
		};

		class ConstIterator {
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = const T*;
			using reference = const T&;
		public:
			ConstIterator() = default;
			~ConstIterator() = default;
			ConstIterator(const ConstIterator&) = default;
			ConstIterator& operator=(const ConstIterator&) = default;
			ConstIterator(const SparseSet* sparseSet, UnsignedInteger index) : sparseSet(sparseSet), index(index) {}
		public:
			ConstIterator operator++(int) {
				ConstIterator res{*this};
				++(*this);
				return res;
			}
			ConstIterator &operator++() {
				++index;
				return *this;
			}
			ConstIterator operator--(int) {
				ConstIterator res{*this};
				--(*this);
				return res;
			}
			ConstIterator &operator--() {
				--index;
				return *this;
			}

			reference operator*() const { return GetElement(); }
			pointer operator->() const { return &GetElement(); }

			bool operator==(const ConstIterator &o) const { return sparseSet == o.sparseSet && index == o.index; }
			bool operator!=(const ConstIterator &o) const { return !(*this == o); }
			auto operator<=>(const ConstIterator &o) const { return index <=> o.index; }
		public:
			UnsignedInteger GetID() const {return sparseSet->dense[index];}
			UnsignedInteger GetIndex() const {return index;}
			reference GetElement() const  {return sparseSet->elements.get(index);}
		private:
			const SparseSet* sparseSet{nullptr};
			UnsignedInteger index{0};
		};

		Iterator begin() {return Iterator{this, 0};}
		Iterator end() {return Iterator(this, dense.size());}

		ConstIterator cbegin() const {return ConstIterator{this, 0};}
		ConstIterator cend() const {return ConstIterator(this, dense.size());}
	public:

		[[nodiscard]] bool Exist(const UnsignedInteger id) const {
			if (id >= sparse.size()) return false;
			const UnsignedInteger index = sparse[id];
			if (index >= dense.size()) return false;
			return dense[index] == id;
		}

		virtual bool Create(const UnsignedInteger id) {
			if (!RawCreate(id)) return false;

			// Initialize the data.
			new (&elements[sparse[id]]) T();

			return true;
		}

		virtual bool Create(const UnsignedInteger id, const T &data) {
			if (!RawCreate(id)) return false;

			// Initialize the data.
			new (&elements[sparse[id]]) T(data);

			return true;
		}

		virtual void Remove(const UnsignedInteger id) {
			// Check the id is used. Cause no point doing work for naught.
			if (!Exist(id)) return;

			UnsignedInteger index = sparse[id];

			const UnsignedInteger last_index = dense.size() - 1;
			// Only doing the expensive bit of work if we need a swap.
			if (index != last_index) {
				// We are not the last one, so to not make any hole, we do a swap and delete.
				const UnsignedInteger swapIndex = last_index;
				const UnsignedInteger swapId = dense[swapIndex];

				MemoryHelper::c_swap_memory<T>(elements[index], elements[swapIndex]);
				MemoryHelper::c_swap_memory<UnsignedInteger>(dense[index], dense[swapIndex]);
				MemoryHelper::c_swap_memory<UnsignedInteger>(sparse[id], sparse[swapId]);

				index = swapIndex;
			}

			// Calling destructor cause the type T is not in use anymore.
			if constexpr (CallDestructorT) {
				elements[index].~T();
			}

			// Removing the index from existing.
			elements.pop_back();
			dense.pop_back();

			// Safely setting the sparse to -1 just in case anything happens.
			// But is probably useless and might be worth removing to remove a useless CPU instruction.
			sparse[id] = -1;
		}

		virtual T& GetOrCreate(const UnsignedInteger id) {
			if (!Exist(id)) {
				Create(id);
			}
			return Get(id);
		}

		virtual T& GetOrCreate(const UnsignedInteger id, const T& data) {
			if (!Exist(id)) {
				Create(id, data);
			}
			return Get(id);
		}

		virtual void Clear() {
			while (!dense.empty()) {
				Remove(dense[dense.size() - 1]);
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

		[[nodiscard]] virtual T *TryGet(const UnsignedInteger id) {
			if (!Exist(id)) return nullptr;
			return &elements[sparse[id]];
		}

		[[nodiscard]] virtual T &Get(const UnsignedInteger id) {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif
			return elements[sparse[id]];
		}
		[[nodiscard]] virtual const T *TryGet(const UnsignedInteger id) const {
			if (!Exist(id)) return nullptr;
			return &elements[sparse[id]];
		}

		[[nodiscard]] virtual const T &Get(const UnsignedInteger id) const {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(Exist(id), "The element ID {} doesn't exist.", id);
#endif
			return elements[sparse[id]];
		}

		UnsignedInteger GetIndex(const UnsignedInteger id) const {
			MGN_CORE_ASSERT(dense[sparse[id]] == id, "ID '{}' is not valid.", id);
			return sparse[id];
		}

		UnsignedInteger GetID(const UnsignedInteger index) const {
			MGN_CORE_ASSERT(sparse[dense[index]] == index, "Index '{}' is not valid.", index);
			return dense[index];
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


	/**
	 * A Raw Sparse Set where we manage the IDs inside the Data Structure using a FreeList and incremental IDs.
	 * @tparam UnsignedInteger The unsigned integer used to count, measure, etc. wherever it's necessary.
	 */
	template<typename T, typename UnsignedInteger = uint32_t, bool CallDestructorT = true>
	class AutoIdSparseSet : public SparseSet<T, UnsignedInteger, CallDestructorT> {
	public:
		AutoIdSparseSet() :
			SparseSet<T, UnsignedInteger, CallDestructorT>() {
			FreeList.reserve(256);
			FreeList.reserve(256);
		}
		explicit AutoIdSparseSet(const UnsignedInteger capacity) :
			SparseSet<T, UnsignedInteger, CallDestructorT>(capacity) {
			FreeList.reserve(capacity);
			FreeList.reserve(capacity);
		}
		AutoIdSparseSet(const AutoIdSparseSet& o) : SparseSet<T, UnsignedInteger, CallDestructorT>(o) {
			FreeList = o.FreeList;
			IDs = o.IDs;
		}
		AutoIdSparseSet& operator=(const AutoIdSparseSet& o) {
			SparseSet<T, UnsignedInteger, CallDestructorT>::operator=(o);
			FreeList = o.FreeList;
			IDs = o.IDs;
			return *this;
		}

		AutoIdSparseSet(AutoIdSparseSet&& aiss) noexcept {swap(aiss);}
		AutoIdSparseSet& operator=(AutoIdSparseSet&& aiss) noexcept {swap(aiss); return *this;}

		virtual ~AutoIdSparseSet() override {
			FreeList.clear();
			IDs = 0;
		}

		void swap(AutoIdSparseSet& aiss) noexcept {
			SparseSet<T, UnsignedInteger, CallDestructorT>::swap(aiss);
			FreeList.swap(aiss.FreeList);
			std::swap(IDs, aiss.IDs);
		}
	private:
		UnsignedInteger CreateID() {
			UnsignedInteger id;
			if (!FreeList.empty()) {
				id = FreeList.back();
				FreeList.pop_back();
			}
			else {
				id = IDs++;
			}
			return id;
		} // This way, std::numeric_limits<UnsignedInteger>::max() is a null id.

		void EnsureFreelistContinuityOnCreate(const UnsignedInteger id) {
			if (id >= IDs) {
				const UnsignedInteger nextIDs = (id + 1);
				const UnsignedInteger numberIdToAddInFreeList = nextIDs - IDs;
				FreeList.reserve(FreeList.size() + numberIdToAddInFreeList + SparseSet<T, UnsignedInteger, CallDestructorT>::c_OverheadResize);
				//FreeList.redimension(FreeList.size() + numberIdToAddInFreeList);
				for (UnsignedInteger i = IDs; i < id; ++i) {
					FreeList.push_back(i);
				}
				IDs = nextIDs;
			}
			else if (!FreeList.empty()) {
				auto begin = FreeList.cbegin();
				auto end = FreeList.cend();

				auto it = std::find(begin, end, id);
				if (it != end) {
					FreeList.swap_and_remove(it);
				}
			}
		}

	public:
		static constexpr UnsignedInteger NullId{std::numeric_limits<UnsignedInteger>::max()};

	public:
		virtual UnsignedInteger Create() {
			const UnsignedInteger id = CreateID();
			const bool result = SparseSet<T, UnsignedInteger, CallDestructorT>::Create(id);
			return result ? id : NullId;
		}

		virtual UnsignedInteger Create(const T &data) {
			const UnsignedInteger id = CreateID();
			const bool result = SparseSet<T, UnsignedInteger, CallDestructorT>::Create(id, data);
			return result ? id : NullId;
		}

		virtual bool Create(const UnsignedInteger id) override {
			if (id == NullId) return false;
			EnsureFreelistContinuityOnCreate(id);
			return SparseSet<T, UnsignedInteger, CallDestructorT>::Create(id);
		}

		virtual bool Create(const UnsignedInteger id, const T &data) override {
			if (id == NullId) return false;
			EnsureFreelistContinuityOnCreate(id);
			return SparseSet<T, UnsignedInteger, CallDestructorT>::Create(id, data);
		}

		virtual T& GetOrCreate(const UnsignedInteger id) override {
			EnsureFreelistContinuityOnCreate(id);
			return SparseSet<T, UnsignedInteger, CallDestructorT>::GetOrCreate(id);
		}

		virtual T& GetOrCreate(const UnsignedInteger id, const T &data) override {
			EnsureFreelistContinuityOnCreate(id);
			return SparseSet<T, UnsignedInteger, CallDestructorT>::GetOrCreate(id, data);
		}

		virtual void Remove(const UnsignedInteger id) override {
			if (id == NullId) return;
			if (!SparseSet<T, UnsignedInteger, CallDestructorT>::Exist(id)) {
				return;
			}
			FreeList.push_back(id);
			SparseSet<T, UnsignedInteger, CallDestructorT>::Remove(id);
		}

		virtual void Clear() override {
			while (!SparseSet<T, UnsignedInteger, CallDestructorT>::dense.empty()) {
				const auto id = SparseSet<T, UnsignedInteger, CallDestructorT>::dense.get(SparseSet<T, UnsignedInteger, CallDestructorT>::dense.size() - 1);
				FreeList.push_back(id);
				SparseSet<T, UnsignedInteger, CallDestructorT>::Remove(id);
			}
		}

	protected:
		HeapArray<UnsignedInteger> FreeList{SparseSet<T, UnsignedInteger, CallDestructorT>::c_OverheadResize};
		UnsignedInteger IDs{0};
	};
} // namespace Imagine::Core
