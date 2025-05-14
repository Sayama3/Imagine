//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Entity.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/RawSparseSet.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core
{


    class Scene {
    public:
        static constexpr uint64_t c_EntityPrepareCount = 1024;
    public:
        Scene();
        ~Scene();
    public:
        EntityID CreateEntity();
        Entity& GetEntity(EntityID id);
        const Entity& GetEntity(EntityID id) const;
        void DestroyEntity(EntityID id);

        void AddComponentType(UUID componentId, uint64_t size, void(*constructor)(void*, UnsignedInteger) = nullptr, void(*destructor)(void*, UnsignedInteger) = nullptr, void(*copy_constructor)(void*, UnsignedInteger, BufferView view) = nullptr);
        UUID AddComponentType(uint64_t size, void(*constructor)(void*, UnsignedInteger) = nullptr, void(*destructor)(void*, UnsignedInteger) = nullptr, void(*copy_constructor)(void*, UnsignedInteger, BufferView view) = nullptr);

        template<typename T>
        void AddComponentType()
        {
            AddComponentType(UUID::FromType<T>(), sizeof(T), (void* data, UnsignedInteger size) { new (data) T(); }, (void* data, UnsignedInteger size) { reinterpret_cast<T*>(data)->~T(); }, (void* data, UnsignedInteger size, BufferView view) { new (data) T(view.Get<T>()); });
        }

        BufferView AddComponent(EntityID entityId, UUID componentId);
        BufferView GetComponent(EntityID entityId, UUID componentId);
        BufferView GetOrAddComponent(EntityID entityId, UUID componentId);

        template<typename T>
        BufferView AddComponent(const EntityID entityId) {auto view = AddComponent(entityId, UUID::FromType<T>()); view.Get<T>() = T{}; return view;}
        template<typename T>
        BufferView GetComponent(const EntityID entityId) {return GetComponent(entityId, UUID::FromType<T>());}
        template<typename T>
        BufferView GetOrAddComponent(const EntityID entityId) { return GetOrAddComponent(entityId, UUID::FromType<T>()); }
    private:
        AutoIdSparseSet<Entity, uint32_t> m_SparseEntities;
        std::unordered_map<UUID, RawSparseSet<uint32_t>> m_CustomComponents;
    };
}
