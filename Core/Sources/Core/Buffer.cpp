//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Core/Buffer.hpp"

namespace Imagine::Core
{
    Buffer::Buffer() : m_Data(nullptr), m_Size(0)
    {
    }

    Buffer::Buffer(void* data, const uint64_t size) :m_Data(data), m_Size(size)
    {
    }

    Buffer::Buffer(const uint64_t size) : m_Data(malloc(size)), m_Size(size)
    {
    }

    Buffer::~Buffer()
    {
        Release();
    }

    Buffer::Buffer(Buffer&& other) noexcept : m_Data(nullptr), m_Size(0)
    {
        swap(other);
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        swap(other);
        return *this;
    }

    void Buffer::Reallocate(const uint64_t size)
    {
        Release();
        m_Data = malloc(size);
        m_Size = size;
    }

    void Buffer::Release()
    {
        free(m_Data);
        m_Data = nullptr;
        m_Size = 0;
    }

    void Buffer::Zeroes()
    {
        if (!m_Data) return;
        memset(m_Data, 0, m_Size);
    }

    void* Buffer::Get()
    {
        return m_Data;
    }

    const void* Buffer::Get() const
    {
        return m_Data;
    }

    uint64_t Buffer::Size() const
    {
        return m_Size;
    }

    void Buffer::swap(Buffer& other) noexcept
    {
        std::swap(m_Data, other.m_Data);
        std::swap(m_Size, other.m_Size);
    }
}
