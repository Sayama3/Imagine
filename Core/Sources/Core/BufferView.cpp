//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Core/BufferView.hpp"

namespace Imagine::Core
{
    BufferView::BufferView() : m_Buffer(nullptr), m_Size(0)
    {
    }

    BufferView::BufferView(Buffer& buffer, const uint64_t offset, const uint64_t size) : m_Buffer(buffer.Get() ? reinterpret_cast<uint8_t*>(buffer.Get()) + offset : nullptr), m_Size(size) {
    }

    BufferView::BufferView(void* buffer, const uint64_t offset, const uint64_t size) : m_Buffer(buffer ? reinterpret_cast<uint8_t*>(buffer) + offset : nullptr), m_Size(size)
    {
    }

    BufferView::~BufferView()
    {
    }

    BufferView::BufferView(const BufferView& other) : m_Buffer(other.m_Buffer), m_Size(other.m_Size)
    {
    }

    BufferView& BufferView::operator=(const BufferView& other) {
        m_Buffer = other.m_Buffer;
        m_Size = other.m_Size;
        return *this;
    }

    bool BufferView::IsValid() const
    {
        return m_Buffer && m_Size;
    }

    BufferView::operator bool() const
    {
        return IsValid();
    }

    void BufferView::Zeroes()
    {
        void* ptr = Get();
        if (!ptr) return;
        memset(ptr, 0, m_Size);
    }

    void* BufferView::Get()
    {
        if (!m_Buffer) return nullptr;
        return m_Buffer;
    }

    const void* BufferView::Get() const
    {
        if (!m_Buffer) return nullptr;
        return m_Buffer;
    }

    uint64_t BufferView::Size() const
    {
        return m_Size;
    }
}

namespace Imagine::Core
{
    ConstBufferView::ConstBufferView() : m_Buffer(nullptr), m_Size(0)
    {
    }

    ConstBufferView::ConstBufferView(const void* buffer, const uint64_t offset, const uint64_t size) : m_Buffer(buffer ? reinterpret_cast<const uint8_t*>(buffer) + offset : nullptr), m_Size(size)
    {
    }

    ConstBufferView::ConstBufferView(const BufferView& view) : m_Buffer(view.Get()), m_Size(view.Size())
    {
    }

    ConstBufferView & ConstBufferView::operator=(const BufferView &view) {
        m_Buffer = view.Get();
        m_Size = view.Size();
        return *this;
    }

    ConstBufferView::~ConstBufferView()
    {
    }

    ConstBufferView::ConstBufferView(const ConstBufferView& other) : m_Buffer(other.m_Buffer), m_Size(other.m_Size)
    {
    }

    ConstBufferView& ConstBufferView::operator=(const ConstBufferView& other) {
        m_Buffer = other.m_Buffer;
        m_Size = other.m_Size;
        return *this;
    }

    bool ConstBufferView::IsValid() const
    {
        return m_Buffer && m_Size;
    }

    ConstBufferView::operator bool() const
    {
        return IsValid();
    }

    const void* ConstBufferView::Get() const
    {
        if (!m_Buffer) return nullptr;
        return reinterpret_cast<const uint8_t*>(m_Buffer);
    }

    uint64_t ConstBufferView::Size() const
    {
        return m_Size;
    }
}
