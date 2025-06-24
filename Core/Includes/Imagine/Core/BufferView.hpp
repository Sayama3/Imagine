//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Buffer.hpp"
#include "Imagine/Core/Macros.hpp"

namespace Imagine::Core {
	/**
	 * A slice of a buffer.
	 */
	class BufferView {
	public:
		template<typename T>
		static BufferView MakeSlice(Buffer *buffer, const uint64_t index) {
			if (!buffer) return BufferView{};
			constexpr uint64_t size = sizeof(T);
			const uint64_t offset = index * size;
			if (offset + size > buffer->Size()) return BufferView{};
			return BufferView{buffer->Get(), offset, size};
		}
		template<typename T>
		inline static BufferView Make(std::vector<T>& vector) {
			if (vector.empty()) return {};
			return {vector.data(), vector.size() * sizeof(T)};
		}

	public:
		BufferView();
		BufferView(Buffer &buffer, uint64_t offset, uint64_t size);
		BufferView(void *buffer, uint64_t offset, uint64_t size);
		~BufferView();
		BufferView(const BufferView &);
		BufferView &operator=(const BufferView &);

	public:
		bool IsValid() const;
		explicit operator bool() const;

	public:
		void Zeroes();

		void *Get();
		const void *Get() const;

		uint64_t Size() const;

		template<typename T>
		T *Get() {
			// static_assert(sizeof(T) == m_Size);
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
			return reinterpret_cast<T *>(Get());
		}

		template<typename T>
		const T *Get() const {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
			return reinterpret_cast<T *>(Get());
		}

		template<typename T>
		T &As() {
			// static_assert(sizeof(T) == m_Size);
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
			return *reinterpret_cast<T *>(Get());
		}

		template<typename T>
		const T &As() const {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
			return *reinterpret_cast<T *>(Get());
		}

	private:
		void *m_Buffer{nullptr};
		uint64_t m_Size;
	};
	/**
	 * A slice of a buffer.
	 */
	class ConstBufferView {
	public:
		template<typename T>
		static ConstBufferView MakeSlice(const Buffer *buffer, const uint64_t index) {
			return MakeSlice(buffer, index * sizeof(T), sizeof(T));
		}

		inline static ConstBufferView MakeSlice(const Buffer *buffer, const uint64_t offset, const uint64_t size) {
			if (!buffer) return ConstBufferView{};
			if (offset + size > buffer->Size()) return ConstBufferView{};
			return ConstBufferView{buffer->Get(), offset, size};
		}

		template<typename T>
		inline static ConstBufferView Make(const std::vector<T>& vector) {
			if (vector.empty()) return {};
			return {vector.data(), vector.size() * sizeof(T)};
		}

	public:
		ConstBufferView();
		ConstBufferView(const void *buffer, uint64_t size);
		ConstBufferView(const void *buffer, uint64_t offset, uint64_t size);
		ConstBufferView(const BufferView &view);
		ConstBufferView &operator=(const BufferView &view);
		~ConstBufferView();
		ConstBufferView(const ConstBufferView &);
		ConstBufferView &operator=(const ConstBufferView &);

	public:
		bool IsValid() const;
		explicit operator bool() const;

		const void *Get() const;

		uint64_t Size() const;
		template<typename  T>

		uint64_t Count() const {
			return Size() / sizeof(T);
		}

		template<typename T>
		const T *Get() const {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
			return reinterpret_cast<const T *>(Get());
		}

		template<typename T>
		const T &As() const {
#ifdef MGN_DEBUG
			MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
			return *reinterpret_cast<const T *>(Get());
		}

	private:
		const void *m_Buffer{nullptr};
		uint64_t m_Size;
	};
} // namespace Imagine::Core
