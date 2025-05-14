//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Buffer.hpp"

namespace Imagine::Core
{
    /**
     * A slice of a buffer.
     */
    class BufferView
    {
    public:
        template<typename T>
        static BufferView MakeSlice(Buffer* buffer, const uint64_t index)
        {
            if (!buffer) return BufferView{};
            constexpr uint64_t size = sizeof(T);
            const uint64_t offset = index * size;
            if (offset + size > buffer->Size()) return BufferView{};
            return BufferView{buffer->Get(), offset, size};
        }
    public:
        BufferView();
        BufferView(void* buffer, uint64_t offset, uint64_t size);
        ~BufferView() override;
        BufferView(const BufferView&);
        BufferView& operator=(const BufferView&);
    public:
        bool IsValid() const;
        explicit operator bool() const;
    public:
        void Zeroes();

        void* Get();
        const void* Get() const;

        uint64_t Size() const;

        template<typename T>
        T& Get()
        {
            // static_assert(sizeof(T) == m_Size);
#ifdef MGN_DEBUG
            MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
            return *reinterpret_cast<T*>(Get());
        }

        template<typename T>
        const T& Get() const
        {
#ifdef MGN_DEBUG
            MGN_CORE_ASSERT(sizeof(T) == m_Size && Get(), "The type or the offset is not valid.");
#endif
            return *reinterpret_cast<T*>(Get());
        }

    private:
        void* m_Buffer{nullptr};
        const uint64_t m_Offset;
        const uint64_t m_Size;
    };
}
