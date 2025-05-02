//
// Created by Iannis on 02/05/2025.
//

#pragma once

namespace Imagine::Core
{
    class BufferEvents
    {
    public:
        virtual ~BufferEvents() = default;
        virtual void OnChangeSize(uint64_t size) = 0;
        virtual void OnBufferDestroy() = 0;
    };

    class Buffer
    {
    public:
        Buffer();
        Buffer(void* data, uint64_t size);
        Buffer(uint64_t size);
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
    private:
        void Release(bool triggerEvents);
    public:
        void AddSubscriber(BufferEvents* subscriber);
        void RemoveSubscriber(BufferEvents* subscriber);
    private:
        void ChangeSize(uint64_t size);
        void Destroy();
    public:
        void* Get();
        const void* Get() const;

        uint64_t Size() const;
    public:
        template<typename T>
        T& Get(const uint64_t index)
        {
            static_assert(m_Size % sizeof(T) == 0);
            // TODO: Maybe use "static_cast" ?
            return reinterpret_cast<T*>(m_Data)[index];
        }

        template<typename T>
        T& operator[](const uint64_t index) {return Get<T>(index);}

        template<typename T>
        const T& Get(const uint64_t index) const
        {
            static_assert(m_Size % sizeof(T) == 0);
            // TODO: Maybe use "static_cast" ?
            return reinterpret_cast<T*>(m_Data)[index];
        }

        template<typename T>
        const T& operator[](const uint64_t index) const {return Get<T>(index);}

        template<typename T>
        uint64_t Count() const
        {
            static_assert(m_Size % sizeof(T) == 0);
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
        uint64_t m_Size;
        std::vector<BufferEvents*> m_Subscribers;
    };
}
