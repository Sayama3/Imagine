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
        m_IDTable.reserve(c_EntityPrepareCount);
        m_Entities.resize(c_EntityPrepareCount);

        AddComponentType<Renderable>();
        AddComponentType<Physicalisable>();

        m_FreeList.resize(c_EntityPrepareCount);

        auto it = m_FreeList.begin();
        for (int i = 0; i < c_EntityPrepareCount; ++i)
        {
            *it++ = i;
        }
    }

    Scene::~Scene() = default;

    Entity Scene::CreateEntity()
    {
        UUID id{};
        const uint64_t index = m_FreeList.front();
        m_FreeList.pop_front();

        m_IDTable[id] = index;
        m_Entities[index] = Entity {
            id,
            Math::Identity<Quat>(),
            Vec3{0},
        };
        return m_Entities.at(index);
    }

    Entity& Scene::GetEntity(const UUID id)
    {
        const uint64_t index = m_IDTable.at(id);
        return m_Entities.at(index);
    }

    const Entity& Scene::GetEntity(const UUID id) const
    {
        const uint64_t index = m_IDTable.at(id);
        return m_Entities.at(index);
    }

    void Scene::DestroyEntity(const UUID id)
    {
        if (!m_IDTable.contains(id)) return;
        const uint64_t index = m_IDTable.at(id);
        m_FreeList.push_front(index);
        m_IDTable.erase(id);
    }

    UUID Scene::AddComponentType(const uint64_t size)
    {
        UUID id{};
        AddComponentType(id, size);
        return id;
    }

    void Scene::AddComponentType(const UUID componentId, const uint64_t size)
    {
        m_CustomComponents[componentId] = Component{c_EntityPrepareCount, size};
    }

    BufferView Scene::AddComponent(const UUID entityId, const UUID componentId)
    {
        BufferView view {GetComponent(entityId, componentId)};
        view.Zeroes();
        return BufferView(view);
    }

    BufferView Scene::GetComponent(const UUID entityId, const UUID componentId)
    {
        if (!m_CustomComponents.contains(componentId))
        {
            return BufferView{};
        }
        if (!m_IDTable.contains(entityId))
        {
            return BufferView{};
        }

        const uint64_t index = m_IDTable.at(entityId);
        auto& component = m_CustomComponents.at(componentId);
        return BufferView{&component.Buffer, index*component.ComponentSize, component.ComponentSize};
    }
}
