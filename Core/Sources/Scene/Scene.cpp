//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Scene/Physicalisable.hpp"
#include "Imagine/Scene/Renderable.hpp"

namespace Imagine::Core {
	Scene::Scene() {
	}

	Scene::~Scene() = default;

	EntityID Scene::CreateEntity() {
		const EntityID id = {m_SparseEntities.Create()};
		m_SparseEntities.Get(id.id).Id = id;
		m_Roots.push_back(id);
		return id;
	}

	Entity &Scene::GetEntity(const EntityID id) {
		return m_SparseEntities.Get(id.id);
	}

	const Entity &Scene::GetEntity(const EntityID id) const {
		return m_SparseEntities.Get(id.id);
	}

	void Scene::DestroyEntity(const EntityID id) {
		m_SparseEntities.Remove(id.id);
		for (auto &[uuid, rawSparseSet]: m_CustomComponents) {
			rawSparseSet.Remove(id.id);
		}
	}

	void Scene::Clear() {
		for (auto &[uuid, rawSparseSet]: m_CustomComponents) {
			rawSparseSet.Clear();
		}
		m_SparseEntities.Clear();
	}
	Scene::RelationshipIterator::RelationshipIterator(const RelationshipIterator &o) :
		scene(o.scene), current(o.current) {
	}
	Scene::RelationshipIterator &Scene::RelationshipIterator::operator=(const RelationshipIterator &o) {
		scene = o.scene;
		current = o.current;
		return *this;
	}
	Scene::RelationshipIterator::RelationshipIterator(Scene *scene, EntityID id) : scene(scene), current(id) {
	}
	// RelationshipIterator
	Entity &Scene::RelationshipIterator::Get() {
		return scene->GetEntity(current);
	}
	const Entity &Scene::RelationshipIterator::Get() const {
		return scene->GetEntity(current);
	}
	bool Scene::RelationshipIterator::IsRoot() const {
		return std::find(scene->m_Roots.cbegin(), scene->m_Roots.cend(), current) != scene->m_Roots.cend();
	}
	bool Scene::RelationshipIterator::HasChildren() const {
		const auto *c = scene->m_Children.TryGet(current.id);
		return c && c->firstChild.IsValid();
	}
	bool Scene::RelationshipIterator::HasParent() const {
		const auto *c = scene->m_Parents.TryGet(current.id);
		return c && c->parent.IsValid();
	}
	uint64_t Scene::RelationshipIterator::CountChildren() const {
		auto *c = scene->m_Children.TryGet(current.id);
		if (!c || !c->firstChild.IsValid()) return 0;

		uint64_t count = 1;
		const Sibling *sibling = scene->m_Siblings.TryGet(c->firstChild.id);
		while (sibling && sibling->next.IsValid()) {
			count += 1;
			sibling = scene->m_Siblings.TryGet(sibling->next.id);
		}
		return count;
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::PreviousSibling() const {
		Sibling* sibling = scene->m_Siblings.TryGet(current.id);
		return sibling ? RelationshipIterator{scene, sibling->previous} : RelationshipIterator{scene, EntityID::NullID};
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::NextSibling() const {
		Sibling *sibling = scene->m_Siblings.TryGet(current.id);
		return sibling ? RelationshipIterator{scene, sibling->next} : RelationshipIterator{scene, EntityID::NullID};
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::Parent() const {
		struct Parent *parent = scene->m_Parents.TryGet(current.id);
		return parent ? RelationshipIterator{scene, parent->parent} : RelationshipIterator{scene, EntityID::NullID};
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::FirstChild() const {
		struct Child *child = scene->m_Children.TryGet(current.id);
		return child ? RelationshipIterator{scene, child->firstChild} : RelationshipIterator{scene, EntityID::NullID};
	}
	Scene::RelationshipIterator &Scene::RelationshipIterator::operator++(int) {
		// First we go see if there is any child.
		struct Child* child = scene->m_Children.TryGet(current.id);
		if (child && child->firstChild.IsValid()) {
			current = child->firstChild;
			return *this;
		}

		// If there is no child we see if we don't have any next sibling.
		struct Sibling *sibling = scene->m_Siblings.TryGet(current.id);
		if (sibling && sibling->next.IsValid()) {
			current = sibling->next;
			return *this;
		}

		// If we don't have any sibling, we go to see it our parent (or the parent of our parent recursively) don't have any next sibling.
		struct Parent* parent = scene->m_Parents.TryGet(current.id);
		while (parent && parent->parent.IsValid()) {
			struct Sibling* parentSibling = scene->m_Siblings.TryGet(parent->parent.id);
			if (parentSibling && parentSibling->next.IsValid()) {
				current = parentSibling->next;
				return *this;
			}
			parent = scene->m_Parents.TryGet(parent->parent.id);
		}

		// If we don't have a child, we don't have a sibling, and no ancestry have any sibling... Well, that's the end.
		current = EntityID::NullID;
		return *this;
	}

	// Relationship management
	void Scene::SetParent(const EntityID entity, const EntityID parent) {
		AddToChild(parent, entity);
	}

	void Scene::AddToChild(const EntityID entity, const EntityID child) {
		auto it = std::find(m_Roots.cbegin(), m_Roots.cend(), child);
		if (it != m_Roots.cend()) {
			m_Roots.swap_and_remove(it);
		}

		Parent &parent = m_Parents.GetOrCreate(child.id);
		if (parent.parent.IsValid()) {
			if (parent.parent == child) {
				// Removing the parent from having child or attaching the next sibling if there is any.
				Sibling *sibling = m_Siblings.TryGet(child.id);
				if (!sibling || !sibling->next.IsValid()) {
					m_Children.Remove(parent.parent.id);
				}
				else {
					m_Children.Get(parent.parent.id).firstChild = sibling->next;
					m_Siblings.Get(sibling->next.id).previous = EntityID::NullID;
				}
			}
			else {
				// Reordering the siblings to preserve the hierarchy as a linked-list would do.
				Sibling &sibling = m_Siblings.Get(child.id);
				Sibling *previous = sibling.previous.IsValid() ? m_Siblings.TryGet(sibling.previous.id) : nullptr;
				Sibling *next = sibling.next.IsValid() ? m_Siblings.TryGet(sibling.next.id) : nullptr;

				if (previous) {
					previous->next = next ? sibling.next : EntityID::NullID;
				}
				if (next) {
					next->previous = previous ? sibling.previous : EntityID::NullID;
				}
			}
		}

		// Finally assigning the new parent;
		parent.parent = entity;

		// Checking for the new parent where is some available place.
		Child &childComponent = m_Children.GetOrCreate(entity.id);
		if (childComponent.firstChild.IsValid()) {
			const EntityID fistChildId = childComponent.firstChild;
			EntityID currentSibling = fistChildId;
			Sibling *sibling = &m_Siblings.GetOrCreate(fistChildId.id);
			while (sibling->next.IsValid()) {
				currentSibling = sibling->next;
				sibling = &m_Siblings.Get(currentSibling.id);
			}
			sibling->next = child;
			m_Siblings.GetOrCreate(child.id).previous = currentSibling.id;
		}
		else {
			childComponent.firstChild = entity;
		}
	}

	// Component management
	UUID Scene::AddComponentType(const uint64_t size, void (*constructor)(void *, uint32_t), void (*destructor)(void *, uint32_t), void (*copy_constructor)(void *, uint32_t, ConstBufferView view)) {
		UUID id{};
		AddComponentType(id, size, constructor, destructor, copy_constructor);
		return id;
	}

	void Scene::AddComponentType(const UUID componentId, const uint64_t size, void (*constructor)(void *, uint32_t), void (*destructor)(void *, uint32_t), void (*copy_constructor)(void *, uint32_t, ConstBufferView view)) {
		m_CustomComponents[componentId] = RawSparseSet<uint32_t>{static_cast<uint32_t>(size), c_EntityPrepareCount};
		auto &components = m_CustomComponents.at(componentId);
		if (constructor) components.SetConstructor(constructor);
		if (destructor) components.SetDestructor(destructor);
		if (copy_constructor) components.SetCopyConstructor(copy_constructor);
	}

	BufferView Scene::AddComponent(const EntityID entityId, const UUID componentId) {
		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
			return BufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);


		if (components.Exist(entityId.id)) {
			return {};
		}

		if (components.Create(entityId.id)) {
			return BufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		MGN_CORE_ERROR("The component id {} already exist on the entity {}.", componentId.string(), entityId.string());
		return {};
	}

	BufferView Scene::AddComponent(const EntityID entityId, const UUID componentId, const ConstBufferView view) {
		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
			return BufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);

		if (components.Exist(entityId.id)) {
			return {};
		}

		if (components.Create(entityId.id, view)) {
			return BufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		MGN_CORE_ERROR("The component id {} already exist on the entity {}.", componentId.string(), entityId.string());
		return {};
	}

	BufferView Scene::GetComponent(const EntityID entityId, const UUID componentId) {
		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
			return BufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);

		if (components.Exist(entityId.id)) {
			return BufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		MGN_CORE_ERROR("The component id {} hasn't been added to the entity {}.", componentId.string(), entityId.string());
		return {};
	}

	ConstBufferView Scene::GetComponent(const EntityID entityId, const UUID componentId) const {
		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
			return ConstBufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);

		if (components.Exist(entityId.id)) {
			return ConstBufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		MGN_CORE_ERROR("The component id {} hasn't been added to the entity {}.", componentId.string(), entityId.string());
		return {};
	}

	BufferView Scene::GetOrAddComponent(EntityID entityId, UUID componentId) {

		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
			return BufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);

		if (components.Exist(entityId.id)) {
			return BufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		if (components.Create(entityId.id)) {
			return BufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		MGN_CORE_ERROR("The component id {} didn't exist on entity {} but somehow hasn't been added successfully either.", componentId.string(), entityId.string());
		return {};
	}

	void Scene::ForEach(std::function<void(const glm::mat4 &worldMat, Entity &entity)>) {
	}
} // namespace Imagine::Core
