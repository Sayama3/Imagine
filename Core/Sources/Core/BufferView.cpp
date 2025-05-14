//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Core/BufferView.hpp"

namespace Imagine::Core
{
    BufferView::BufferView() : m_Buffer(nullptr), m_Offset(0), m_Size(0)
    {
    }

    BufferView::BufferView(Buffer* buffer, const uint64_t offset, const uint64_t size) : m_Buffer(buffer), m_Offset(offset), m_Size(size)
    {
    }

    BufferView::~BufferView()
    {
    }

    BufferView::BufferView(const BufferView& other) : m_Buffer(other.m_Buffer), m_Size(other.m_Size), m_Offset(other.m_Offset)
    {
    }

    BufferView& BufferView::operator=(const BufferView& other) {
        m_Buffer = other.m_Buffer;
        m_Offset = other.m_Offset;
        m_Size = other.m_Size;
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
        return reinterpret_cast<uint8_t*>(m_Buffer->Get()) + m_Offset;
    }

    const void* BufferView::Get() const
    {
        if (!m_Buffer) return nullptr;
        return reinterpret_cast<uint8_t*>(m_Buffer->Get()) + m_Offset;
    }

    uint64_t BufferView::Size() const
    {
        return m_Size;
    }
}
