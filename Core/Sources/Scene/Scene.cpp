//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Scene/Physicalisable.hpp"
#include "Imagine/Scene/Renderable.hpp"

namespace Imagine::Core {
	Scene::Scene() {
		RegisterType<Renderable>();
		RegisterType<Physicalisable>();
	}

	Scene::~Scene() = default;
	Scene::Scene(Scene &&o) noexcept {
		swap(o);
	}
	Scene &Scene::operator=(Scene &&s) noexcept {
		swap(s);
		return *this;
	}
	void Scene::swap(Scene &s) noexcept {
		m_SparseEntities.swap(s.m_SparseEntities);
		m_CustomComponents.swap(s.m_CustomComponents);
		m_Roots.swap(s.m_Roots);
		m_Parents.swap(s.m_Parents);
		m_Children.swap(s.m_Children);
		m_Siblings.swap(s.m_Siblings);
		std::swap(ID, s.ID);
	}

	Scene Scene::Duplicate() const {
		Scene scene{*this};
		scene.ID = UUID{};
		return scene;
	}

	EntityID Scene::CreateEntity() {
		const EntityID id = {m_SparseEntities.Create()};
		m_SparseEntities.Get(id.id).Id = id;
		m_Names.Create(id.id, "Entity " + id.string());
		m_Roots.insert(id);
		return id;
	}

	EntityID Scene::CreateEntity(EntityID parentId) {
		const EntityID id = {m_SparseEntities.Create()};
		m_SparseEntities.Get(id.id).Id = id;
		m_Names.Create(id.id, "Entity " + id.string());
		if (parentId.IsValid()) {
			AddToChild(parentId, id);
		} else {
			m_Roots.insert(id);
		}
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
		m_Names.Remove(id.id);
		//TODO: Delete all children too
		//TODO: Reorder the sibling hierarchy
	}

	void Scene::Clear() {
		for (auto &[uuid, rawSparseSet]: m_CustomComponents) {
			rawSparseSet.Clear();
		}
		m_SparseEntities.Clear();
	}
	std::string Scene::GetName(EntityID entityId) const {
		auto* name = m_Names.TryGet(entityId.id);
		MGN_CHECK_ERROR(name, "The entity id {} don't have name.", entityId.id);
		return name ? *name : std::string{};
	}
	void Scene::SetName(EntityID entityId, std::string name) {
		m_Names.GetOrCreate(entityId.id) = std::move(name);
	}
	Scene::ChildIterator::ChildIterator(Scene *scene, EntityID parent) :
		scene(scene) {
		if (!scene) return;
		auto *child = scene->m_Children.TryGet(parent.id);
		if (child && child->firstChild.IsValid()) {
			currentChild = child->firstChild;
		}
	}
	Scene::ChildIterator Scene::ChildIterator::Next() const {
		ChildIterator sibling{*this};
		++sibling;
		return sibling;
	}
	Scene::ChildIterator Scene::ChildIterator::Previous() const {
		ChildIterator sibling{*this};
		--sibling;
		return sibling;
	}
	Scene::ChildIterator Scene::ChildIterator::operator++(int) {
		ChildIterator res{*this};
		++(*this);
		return res;
	}
	EntityID Scene::ChildIterator::GetParent() const {
		auto *parent = scene->m_Parents.TryGet(currentChild.id);
		return parent ? parent->parent : EntityID::NullID;
	}

	Scene::ChildIterator &Scene::ChildIterator::operator++() {
		auto *sib = scene->m_Siblings.TryGet(currentChild.id);
		if (sib && sib->next.IsValid()) {
			currentChild = sib->next;
		}
		else {
			currentChild = EntityID::NullID;
		}
		return *this;
	}
	Scene::ChildIterator Scene::ChildIterator::operator--(int) {
		ChildIterator res{*this};
		--(*this);
		return res;
	}

	Scene::ChildIterator &Scene::ChildIterator::operator--() {
		auto *sib = scene->m_Siblings.TryGet(currentChild.id);
		if (sib && sib->previous.IsValid()) {
			currentChild = sib->previous;
		}
		else {
			currentChild = EntityID::NullID;
		}
		return *this;
	}

	Scene::ChildIterator Scene::BeginChild(EntityID parent) {
		return ChildIterator{this, parent};
	}

	Scene::ChildIterator Scene::EndChild() {
		return {this, EntityID::NullID};
	}

	Scene::RelationshipIterator::RelationshipIterator(const RelationshipIterator &o) :
		scene(o.scene), current(o.current) {
	}
	Scene::RelationshipIterator &Scene::RelationshipIterator::operator=(const RelationshipIterator &o) {
		scene = o.scene;
		current = o.current;
		return *this;
	}
	Scene::RelationshipIterator::RelationshipIterator(Scene *scene, EntityID id) :
		scene(scene), current(id) {
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
		Sibling *sibling = scene->m_Siblings.TryGet(current.id);
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
	Scene::RelationshipIterator &Scene::RelationshipIterator::operator++() {
		// First we go see if there is any child.
		struct Child *child = scene->m_Children.TryGet(current.id);
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
		struct Parent *parent = scene->m_Parents.TryGet(current.id);
		while (parent && parent->parent.IsValid()) {
			struct Sibling *parentSibling = scene->m_Siblings.TryGet(parent->parent.id);
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

	Scene::RelationshipIterator Scene::BeginRelationship(EntityID e) {
		return Scene::RelationshipIterator{this, e};
	}

	Scene::RelationshipIterator Scene::EndRelationship() {
		return Scene::RelationshipIterator{this, EntityID::NullID};
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::operator++(int) {
		Scene::RelationshipIterator it{*this};
		++*this;
		return it;
	}

	// Relationship management
	void Scene::SetParent(const EntityID entity, const EntityID parent) {
		if (parent.IsValid())
			AddToChild(parent, entity);
	}

	void Scene::AddToChild(const EntityID entity, const EntityID child) {
		if (!child.IsValid()) return;

		// Make the child an orphan
		RemoveParent(child);

		if (entity.IsValid()) {
			// Now that the parent is valid, we can assign the orphan to the new parent.
			AddChild(entity, child);

			// Assigning the new parent;
			m_Parents.GetOrCreate(child.id).parent = entity;

			auto it = std::find(m_Roots.cbegin(), m_Roots.cend(), child);
			if (it != m_Roots.cend()) {
				m_Roots.erase(it);
			}
		}
		else {
			m_Roots.insert(child.id);
		}
	}

	void Scene::RemoveParent(EntityID child) {
		Parent *pParent = m_Parents.TryGet(child.id);
		if (pParent) {
			Parent &parent = *pParent;
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
			m_Parents.Remove(child.id);
		}
	}

	void Scene::AddChild(EntityID parent, EntityID orphan) {
		// Checking for the new parent where is some available place.
		Child &childComponent = m_Children.GetOrCreate(parent.id);
		if (childComponent.firstChild.IsValid()) {
			const EntityID fistChildId = childComponent.firstChild;
			EntityID currentSibling = fistChildId;
			Sibling *sibling = &m_Siblings.GetOrCreate(fistChildId.id);
			while (sibling->next.IsValid()) {
				currentSibling = sibling->next;
				sibling = &m_Siblings.Get(currentSibling.id);
			}
			sibling->next = orphan;
			m_Siblings.GetOrCreate(orphan.id).previous = currentSibling.id;
		}
		else {
			childComponent.firstChild = orphan;
		}
	}

	// Component management
	UUID Scene::RegisterType(std::string name, const uint64_t size, void (*constructor)(void *, uint32_t), void (*destructor)(void *, uint32_t), void (*copy_constructor)(void *, uint32_t, ConstBufferView view)) {
		UUID id{};
		RegisterType(std::move(name), id, size, constructor, destructor, copy_constructor);
		return id;
	}

	void Scene::RegisterType(std::string name, const UUID componentId, const uint64_t size, void (*constructor)(void *, uint32_t), void (*destructor)(void *, uint32_t), void (*copy_constructor)(void *, uint32_t, ConstBufferView view)) {
		m_CustomComponentsMetadata[componentId] = {
			std::move(name),
			componentId,
			size,
			constructor != nullptr,
			destructor != nullptr,
			copy_constructor != nullptr,
		};

		m_CustomComponents[componentId] = RawSparseSet<uint32_t>{static_cast<uint32_t>(size), c_EntityPrepareCount};
		auto &components = m_CustomComponents.at(componentId);
		if (constructor) components.SetConstructor(constructor);
		if (destructor) components.SetDestructor(destructor);
		if (copy_constructor) components.SetCopyConstructor(copy_constructor);
	}

	BufferView Scene::AddComponent(const EntityID entityId, const UUID componentId) {
		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component {} doesn't exist.",componentId.string());
			return BufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);


		if (components.Exist(entityId.id)) {
			return {};
		}

		if (components.Create(entityId.id)) {
			return BufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		MGN_CORE_ERROR("The component '{}' already exist on the entity '{}'.", m_CustomComponentsMetadata.at(componentId).name, GetName(entityId));
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

		MGN_CORE_ERROR("The component '{}' already exist on the entity '{}'.", m_CustomComponentsMetadata.at(componentId).name, GetName(entityId));
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

		MGN_CORE_ERROR("The component '{}' hasn't been added to the entity '{}'.", m_CustomComponentsMetadata.at(componentId).name, GetName(entityId));
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

		MGN_CORE_ERROR("The component '{}' hasn't been added to the entity '{}'.", m_CustomComponentsMetadata.at(componentId).name, GetName(entityId));
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

		MGN_CORE_ERROR("The component '{}' didn't exist on entity '{}' but somehow hasn't been added successfully either.", m_CustomComponentsMetadata.at(componentId).name, GetName(entityId));
		return {};
	}
	bool Scene::HasComponent(EntityID entityId, UUID componentId) const {
		if (!m_CustomComponents.contains(componentId)) {return false;}
		auto &components = m_CustomComponents.at(componentId);
		return components.Exist(entityId.id);
	}
} // namespace Imagine::Core
