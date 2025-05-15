//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Scene/Renderable.hpp"
#include "Imagine/Scene/Physicalisable.hpp"

namespace Imagine::Core
{
    Scene::Scene()
    {
        AddComponentType<Renderable>();
        AddComponentType<Physicalisable>();
    }

    Scene::~Scene() = default;

    EntityID Scene::CreateEntity()
    {
        const EntityID id = { m_SparseEntities.Create() };
        m_SparseEntities.Get(id.id).Id = id;
        return id;
    }

    Entity& Scene::GetEntity(const EntityID id)
    {
        return m_SparseEntities.Get(id.id);
    }

    const Entity& Scene::GetEntity(const EntityID id) const
    {
        return m_SparseEntities.Get(id.id);
    }

    void Scene::DestroyEntity(const EntityID id)
    {
        m_SparseEntities.Remove(id.id);
        for (auto& [uuid, rawSparseSet] : m_CustomComponents)
        {
            rawSparseSet.Remove(id.id);
        }
    }

    UUID Scene::AddComponentType(const uint64_t size, void(*constructor)(void*, uint32_t), void(*destructor)(void*, uint32_t), void(*copy_constructor)(void*, uint32_t, BufferView view))
    {
        UUID id{};
        AddComponentType(id, size, constructor, destructor, copy_constructor);
        return id;
    }

    void Scene::AddComponentType(const UUID componentId, const uint64_t size, void(*constructor)(void*, uint32_t), void(*destructor)(void*, uint32_t), void(*copy_constructor)(void*, uint32_t, BufferView view))
    {
        m_CustomComponents[componentId] = RawSparseSet<uint32_t>{static_cast<uint32_t>(size), c_EntityPrepareCount};
        auto& components = m_CustomComponents.at(componentId);
        if (constructor) components.SetConstructor(constructor);
        if (destructor) components.SetDestructor(destructor);
        if (copy_constructor) components.SetCopyConstructor(copy_constructor);
    }

    BufferView Scene::AddComponent(const EntityID entityId, const UUID componentId)
    {
        if (!m_CustomComponents.contains(componentId))
        {
            MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
            return BufferView{};
        }

        auto& components = m_CustomComponents.at(componentId);


        if (components.Exist(entityId.id)) {
            return {};
        }

        if(components.Create(entityId.id)) {
            return BufferView{ components.Get(entityId.id), 0, components.GetDataSize() };
        }

        MGN_CORE_ERROR("The component id {} already exist on the entity {}.", componentId.string(), entityId.string());
        return {};
    }

    BufferView Scene::GetComponent(const EntityID entityId, const UUID componentId)
    {
        if (!m_CustomComponents.contains(componentId))
        {
            MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
            return BufferView{};
        }

        auto& components = m_CustomComponents.at(componentId);

        if (components.Exist(entityId.id)) {
            return BufferView{ components.Get(entityId.id), 0, components.GetDataSize() };
        }

        MGN_CORE_ERROR("The component id {} hasn't been added to the entity {}.", componentId.string(), entityId.string());
        return {};
    }


    BufferView Scene::GetOrAddComponent(EntityID entityId, UUID componentId) {

        if (!m_CustomComponents.contains(componentId))
        {
            MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
            return BufferView{};
        }

        auto& components = m_CustomComponents.at(componentId);

        if (components.Exist(entityId.id)) {
            return BufferView{ components.Get(entityId.id), 0, components.GetDataSize() };
        }

        if (components.Create(entityId.id)) {
            return BufferView{ components.Get(entityId.id), 0, components.GetDataSize() };
        }

        MGN_CORE_ERROR("The component id {} didn't exist on entity {} but somehow hasn't been added successfully either.", componentId.string(), entityId.string());
        return {};
    }
}
