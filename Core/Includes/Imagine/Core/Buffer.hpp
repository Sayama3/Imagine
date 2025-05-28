//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"

namespace Imagine::Core
{
    class Buffer
    {
    public:
        inline static Buffer Copy(void* data, const uint64_t size) {
            void* newBuff = malloc(size);
            memcpy(newBuff, data, size);
            return Buffer{newBuff, size};
        }

        inline static Buffer Copy(const Buffer& buffer) {
            void* newBuff = malloc(buffer.Size());
            memcpy(newBuff, buffer.Get(), buffer.Size());
            return Buffer{newBuff, buffer.Size()};
        }

        template<typename T>
        inline static Buffer Copy(const T& other) {
            void* newBuff = reinterpret_cast<void*>(new T(other));
            return Buffer{newBuff, sizeof(T)};
        }
    public:
        Buffer();
        Buffer(void* data, uint64_t size);
        explicit Buffer(uint64_t size);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&&) noexcept;
        Buffer& operator=(Buffer&&) noexcept;
    public:
        void Reallocate(uint64_t size);
        void Release();
        /**
         * Reset the bloc of memory to 0 for the size.
         * If no memory allocated, return without doing anything.
         */
        void Zeroes();
    public:
        void* Get();
        const void* Get() const;

        uint64_t Size() const;
    public:
        template<typename T>
        T* Get()
        {
#if MGN_DEBUG
            MGN_CORE_ASSERT(m_Size >= sizeof(T), "Assertion 'm_Size >= sizeof({})' failed.\n  m_Size = '{}' \n  sizeof({}) = '{}'.", typeid(T).name(), typeid(T).name(), m_Size, sizeof(T));
#endif
            // TODO: Maybe use "static_cast" ?
            return reinterpret_cast<T*>(m_Data);
        }

        template<typename T>
        const T* Get() const
        {
#if MGN_DEBUG
            MGN_CORE_ASSERT(m_Size >= sizeof(T), "Assertion 'm_Size >= sizeof({})' failed.\n  m_Size = '{}' \n  sizeof({}) = '{}'.", typeid(T).name(), m_Size, typeid(T).name(), sizeof(T));
#endif
            // TODO: Maybe use "static_cast" ?
            return reinterpret_cast<const T*>(m_Data);
        }

        template<typename T>
        T& As()
        {
#if MGN_DEBUG
            MGN_CORE_ASSERT(m_Size >= sizeof(T), "Assertion 'm_Size >= sizeof({})' failed.\n  m_Size = '{}' \n  sizeof({}) = '{}'.", typeid(T).name(), m_Size, typeid(T).name(), sizeof(T));
#endif
            // TODO: Maybe use "static_cast" ?
            return *reinterpret_cast<T*>(m_Data);
        }

        template<typename T>
        const T& As() const
        {
#if MGN_DEBUG
            MGN_CORE_ASSERT(m_Size >= sizeof(T), "Assertion 'm_Size >= sizeof({})' failed.\n  m_Size = '{}' \n  sizeof({}) = '{}'.", typeid(T).name(), m_Size, typeid(T).name(), sizeof(T));
#endif
            // TODO: Maybe use "static_cast" ?
            return *reinterpret_cast<const T*>(m_Data);
        }

        template<typename T>
        T& Get(const uint64_t index)
        {
#if MGN_DEBUG
            MGN_CORE_ASSERT(m_Size % sizeof(T) == 0, "Assertion 'm_Size % sizeof({}) == 0' failed.\n  m_Size = '{}' \n  sizeof({}) = '{}'.", typeid(T).name(), m_Size, typeid(T).name(), sizeof(T));
            MGN_CORE_ASSERT(index < Count<T>(), "Assertion 'index < Count<{}>()' failed.\n  index = '{}' \n  Count<{}>() = '{}'.", typeid(T).name(), index, typeid(T).name(), Count<T>());
#endif
            return reinterpret_cast<T*>(m_Data)[index];
        }

        template<typename T>
        const T& Get(const uint64_t index) const
        {
#if MGN_DEBUG
        	MGN_CORE_ASSERT(m_Size % sizeof(T) == 0, "Assertion 'm_Size % sizeof({}) == 0' failed.\n  m_Size = '{}' \n  sizeof({}) = '{}'.", typeid(T).name(), m_Size, typeid(T).name(), sizeof(T));
        	MGN_CORE_ASSERT(index < Count<T>(), "Assertion 'index < Count<{}>()' failed.\n  index = '{}' \n  Count<{}>() = '{}'.", typeid(T).name(), index, typeid(T).name(), Count<T>());
#endif
            return reinterpret_cast<const T*>(m_Data)[index];
        }

        template<typename T>
        T& operator[](const uint64_t index) { return Get<T>(index); }

        template<typename T>
        const T& operator[](const uint64_t index) const { return Get<T>(index); }

        template<typename T>
        uint64_t Count() const
        {
            MGN_CORE_ASSERT(m_Size % sizeof(T) == 0, "The size of '{}' cannot give a whole number of object.", typeid(T).name());
            if (!m_Data) return 0;
            return m_Size / sizeof(T);
        }

        template<typename T>
        T* TryGet(const uint64_t index)
        {
            static_assert(m_Size % sizeof(T) == 0);
            if (!m_Data) return nullptr;
            const uint64_t Count = m_Size / sizeof(T);
            if (index >= Count) return nullptr;
            // TODO: Maybe use "static_cast" ?
            return reinterpret_cast<T*>(m_Data)[index];
        }

        template<typename T>
        const T* TryGet(const uint64_t index) const
        {
            static_assert(m_Size % sizeof(T) == 0);
            if (!m_Data) return nullptr;
            const uint64_t Count = m_Size / sizeof(T);
            if (index >= Count) return nullptr;
            // TODO: Maybe use "static_cast" ?
            return reinterpret_cast<T*>(m_Data)[index];
        }
    public:
        void swap(Buffer& other) noexcept;
    public:
    private:
        void* m_Data{nullptr};
        uint64_t m_Size{ 0 };
    };
}
