//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Entity.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/HeapArray.hpp"
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
        Entity CreateEntity();
        Entity& GetEntity(EntityID id);
        const Entity& GetEntity(EntityID id) const;
        void DestroyEntity(EntityID id);

        void AddComponentType(UUID componentId, uint64_t size);
        UUID AddComponentType(uint64_t size);

        template<typename T>
        void AddComponentType()
        {
            AddComponentType(UUID{typeid(T).hash_code()}, sizeof(T));
        }

        BufferView AddComponent(EntityID entityId, UUID componentId);
        BufferView GetComponent(EntityID entityId, UUID componentId);

        template<typename T>
        BufferView AddComponent(const EntityID entityId) {auto view = AddComponent(entityId, UUID(typeid(T).hash_code())); view.Get<T>() = T{}; return view;}
        template<typename T>
        BufferView GetComponent(const EntityID entityId) {return GetComponent(entityId, UUID{typeid(T).hash_code()});}
    private:
        AutoIdSparseSet<Entity> m_SparseEntities;
        std::unordered_map<UUID, RawSparseSet> m_CustomComponents;
    };
}
