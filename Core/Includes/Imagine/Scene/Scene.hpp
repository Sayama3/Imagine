//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Entity.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core
{
    struct Component
    {
        Component() : Buffer{}, ComponentSize{0} {}
        Component(Buffer&& buffer, const uint64_t componentSize) : Buffer(std::move(buffer)), ComponentSize(componentSize) {}
        Component(const uint64_t count, const uint64_t componentSize) : Buffer(componentSize*count), ComponentSize(componentSize) {}
        template<typename T>
        explicit Component(const uint64_t count) : Buffer(sizeof(T)*count), ComponentSize(sizeof(T)) {}

        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;

        Component(Component&& other) noexcept {swap(other);}
        Component& operator=(Component&& other) noexcept {swap(other);return *this;}

        ~Component() = default;

        void swap(Component& other) noexcept {std::swap(Buffer, other.Buffer);std::swap(ComponentSize, other.ComponentSize);}

        Buffer Buffer;
        uint64_t ComponentSize;
    };

    class Scene {
    public:
        static constexpr uint64_t c_EntityPrepareCount = 1024;
    public:
        Scene();
        ~Scene();
    public:
        Entity CreateEntity();
        Entity& GetEntity(UUID id);
        const Entity& GetEntity(UUID id) const;
        void DestroyEntity(UUID id);

        void AddComponentType(UUID componentId, uint64_t size);
        UUID AddComponentType(uint64_t size);

        template<typename T>
        void AddComponentType()
        {
            AddComponentType(UUID{typeid(T).hash_code()}, sizeof(T));
        }

        BufferView AddComponent(UUID entityId, UUID componentId);
        BufferView GetComponent(UUID entityId, UUID componentId);

        template<typename T>
        BufferView AddComponent(const UUID entityId) {auto view = AddComponent(entityId, UUID(typeid(T).hash_code())); view.Get<T>() = T{}; return view;}
        template<typename T>
        BufferView GetComponent(const UUID entityId) {return GetComponent(entityId, UUID{typeid(T).hash_code()});}
    private:
        std::unordered_map<UUID, uint64_t> m_IDTable;
        std::vector<Entity> m_Entities;
        std::unordered_map<UUID, Component> m_CustomComponents;
        std::list<uint64_t> m_FreeList;
    };
}
