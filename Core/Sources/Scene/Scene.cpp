//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Components/Physicalisable.hpp"
#include "Imagine/Components/Renderable.hpp"
#include "Imagine/Layers/PhysicsLayer.hpp"
#include "Imagine/Rendering/Light.hpp"
#include "Imagine/Scene/SceneSerializer.hpp"
#include "Imagine/ThirdParty/FileDialogs.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

#include <utility>
#endif
namespace Imagine {
	Scene::Scene() : Asset() {
		RegisterType<Renderable>();
		RegisterType<Physicalisable>();
		RegisterType<Light>();
	}

	Scene::~Scene() = default;

	Scene Scene::Duplicate() const {
		Scene scene{*this};
		scene.Handle = AssetHandle{};
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
		}
		else {
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
	bool Scene::Exist(EntityID id) const {
		return m_SparseEntities.Exist(id.id);
	}

	void Scene::DestroyEntity(const EntityID entityToRemove) {
		std::vector<EntityID> toRemove{};
		RemoveParent(entityToRemove);
		auto it = BeginRelationship(entityToRemove);
		auto end = EndRelationship();
		while (it != end) {
			const auto id = (it++).GetID();
			toRemove.push_back(id);
			m_Roots.erase(id);
		}

		for (EntityID id: toRemove) {
			m_SparseEntities.Remove(id.id);
			for (auto &[uuid, rawSparseSet]: m_CustomComponents) {
				rawSparseSet.Remove(id.id);
			}
			m_Names.Remove(id.id);
			m_Siblings.Remove(id.id);
			m_Parents.Remove(id.id);
			m_Children.Remove(id.id);
			m_WorldTransform.Remove(id.id);
		}
	}

	void Scene::Clear() {
		for (auto &[uuid, rawSparseSet]: m_CustomComponents) {
			rawSparseSet.Clear();
		}
		m_SparseEntities.Clear();
	}
	std::string Scene::GetName(EntityID entityId) const {
		auto *name = m_Names.TryGet(entityId.id);
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
		scene(o.scene), current(o.current), rootParent(o.rootParent) {
	}
	Scene::RelationshipIterator &Scene::RelationshipIterator::operator=(const RelationshipIterator &o) {
		scene = o.scene;
		current = o.current;
		rootParent = o.rootParent;
		return *this;
	}

	Scene::RelationshipIterator::RelationshipIterator(Scene *scene, EntityID id) :
		scene(scene), current(id) {
		if (!scene) return;
		const auto *c = scene->m_Parents.TryGet(current.id);
		if (c && c->parent.IsValid()) {
			rootParent = c->parent;
		}
	}

	Scene::RelationshipIterator::RelationshipIterator(Scene *scene, EntityID id, EntityID parent) :
		scene(scene), current(id), rootParent(parent) {
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
		return sibling ? RelationshipIterator{scene, sibling->previous, rootParent} : RelationshipIterator{scene, EntityID::NullID, rootParent};
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::NextSibling() const {
		Sibling *sibling = scene->m_Siblings.TryGet(current.id);
		return sibling ? RelationshipIterator{scene, sibling->next, rootParent} : RelationshipIterator{scene, EntityID::NullID, rootParent};
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::Parent() const {
		struct Parent *parent = scene->m_Parents.TryGet(current.id);
		return parent ? RelationshipIterator{scene, parent->parent, rootParent} : RelationshipIterator{scene, EntityID::NullID, rootParent};
	}

	Scene::RelationshipIterator Scene::RelationshipIterator::FirstChild() const {
		struct Child *child = scene->m_Children.TryGet(current.id);
		return child ? RelationshipIterator{scene, child->firstChild, rootParent} : RelationshipIterator{scene, EntityID::NullID, rootParent};
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
		while (parent && parent->parent.IsValid() && parent->parent != rootParent) {

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

	void Scene::MoveToRoot(EntityID entity) {
		RemoveParent(entity);
		m_Roots.insert(entity);
	}

	void Scene::RemoveParent(EntityID child) {
		Parent *pParent = m_Parents.TryGet(child.id);
		if (pParent) {
			Parent &parent = *pParent;
			if (parent.parent.IsValid()) {
				Child &firstChild = m_Children.Get(parent.parent.id);
				if (firstChild.firstChild == child) {
					// Removing the parent from having child or attaching the next sibling if there is any.
					Sibling *sibling = m_Siblings.TryGet(child.id);
					if (!sibling || !sibling->next.IsValid()) {
						m_Children.Remove(parent.parent.id);
					}
					else {
						m_Children.Get(parent.parent.id).firstChild = sibling->next;
						m_Siblings.Get(sibling->next.id).previous = EntityID::NullID;
						m_Siblings.Remove(child.id);
					}
				}
				else {
					// Reordering the siblings to preserve the hierarchy as a linked-list would do.
					Sibling *pSibling = m_Siblings.TryGet(child.id);
					if (pSibling) {
						Sibling &sibling = *pSibling;
						Sibling *previous = sibling.previous.IsValid() ? m_Siblings.TryGet(sibling.previous.id) : nullptr;
						Sibling *next = sibling.next.IsValid() ? m_Siblings.TryGet(sibling.next.id) : nullptr;

						if (previous) {
							previous->next = next ? sibling.next : EntityID::NullID;
						}
						if (next) {
							next->previous = previous ? sibling.previous : EntityID::NullID;
						}
						m_Siblings.Remove(child.id);
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

	void Scene::CacheTransforms() {
		TransformR trs{};
		ForEach<TransformR>(trs, [](const TransformR &parentTrs, Scene *scene, const EntityID id) {
			const TransformR worldTrs{scene->GetEntity(id).LocalPosition, scene->GetEntity(id).LocalRotation, scene->GetEntity(id).LocalScale, parentTrs.PositionLocalToWorld};
			scene->m_WorldTransform.GetOrCreate(id.id) = worldTrs;
			return worldTrs;
		});
	}
	Mat4 Scene::GetWorldTransform(const EntityID id) const {
		const TransformR *trs = m_WorldTransform.TryGet(id.id);
		return trs ? trs->PositionLocalToWorld : Mat4(0);
	}

	TransformR Scene::GetTransform(const EntityID id) const {
		const TransformR *trs = m_WorldTransform.TryGet(id.id);
		return trs ? *trs : TransformR();
	}
	void Scene::SendImGuiCommands() {
#ifdef MGN_IMGUI
		{
			ImGui::Begin("Main Scene");
			if (ImGui::Button("Save")) {
				std::string folder = ThirdParty::FileDialogs::SelectFolder("Load Scene");
				SceneSerializer::SerializeReadable(this, folder);
			}

			if (ImGui::Button("Load")) {
				std::string folder = ThirdParty::FileDialogs::SelectFolder("Load Scene");
				if (std::filesystem::exists(folder))
				{
					Scene* scene = SceneSerializer::DeserializeReadable(folder);
					if (scene) {
						*this = *scene;
						delete scene;
					}
				}
			}
			ImGui::End();
		}

		{
			// const std::string HierarchyName = "Hierarchy##" + Handle.string();
			const std::string HierarchyName = "Hierarchy";
			ImGui::Begin(HierarchyName.c_str());
			{
				static std::string newName = "New Entity";
				ImGui::InputText("##New Entity", &newName);
				ImGui::SameLine();
				if (ImGui::Button("Create")) {
					auto newId = CreateEntity();
					m_Names.GetOrCreate(newId.id) = newName;
				}

				ImGui::Separator();
				auto beg = m_Roots.begin();
				auto end = m_Roots.end();
				for (auto it = beg; it != end; ++it) {
					EntityID entity = *it;
					const bool hasChildren = m_Children.Exist(entity.id);
					const auto id = entity.string();
					uint32_t flags = ImGuiTreeNodeFlags_None;
					if (entity == m_SelectedEntity) flags |= ImGuiTreeNodeFlags_Selected;
					if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf;
					if (ImGui::TreeNodeEx(id.c_str(), flags, m_Names.Get(entity.id).c_str())) {
						if (ImGui::IsItemFocused())
							m_SelectedEntity = entity;
						if (hasChildren) DrawChildren(entity);
						ImGui::TreePop();
					}
				}
			}
			ImGui::End();
		}

		{
			// const std::string PropertiesName = "Properties##" + Handle.string();
			const std::string PropertiesName = "Properties";
			ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
			ImGui::Begin(PropertiesName.c_str());
			{
				if (m_SelectedEntity.IsValid() && Exist(m_SelectedEntity)) {
					ImGui::LabelText("ID", "%u", m_SelectedEntity.id);
					ImGui::InputText("Name", &m_Names.GetOrCreate(m_SelectedEntity.id));

					ImGui::SeparatorText("Transform");
					{
						Entity &e = GetEntity(m_SelectedEntity);
						static auto degRot = glm::eulerAngles(e.LocalRotation) * Math::RadToDeg;
						if (HasComponent<Physicalisable>(m_SelectedEntity) && PhysicsLayer::IsSimulating()) {
							ImGui::BeginDisabled(true);
							ImGui::DragFloat3("Position", Math::ValuePtr(e.LocalPosition), 0.1, 0, 0, "%.3f");
							degRot = glm::eulerAngles(e.LocalRotation) * Math::RadToDeg;
							ImGui::DragFloat3("Rotation", Math::ValuePtr(degRot), 1, 0, 0, "%.2f");
							ImGui::DragFloat3("Scale", Math::ValuePtr(e.LocalScale), 0.1, 0, 0, "%.3f");
							ImGui::EndDisabled();
						}
						else {
							ImGui::DragFloat3("Position", Math::ValuePtr(e.LocalPosition), 0.1, 0, 0, "%.3f");
							if (ImGui::DragFloat3("Rotation", Math::ValuePtr(degRot), 1, 0, 0, "%.2f")) {
								e.LocalRotation = Math::Normalize(Quat(degRot * Math::DegToRad));
							}
							if (ImGui::IsItemDeactivatedAfterEdit()) {
								degRot = glm::eulerAngles(e.LocalRotation) * Math::RadToDeg;
							}
							ImGui::DragFloat3("Scale", Math::ValuePtr(e.LocalScale), 0.1, 0, 0, "%.3f");
						}
					}
					ImGui::Separator();
					if (ImGui::Button("Add Component"))
						ImGui::OpenPopup("add_component_popup");

					for (auto &[id, cc]: m_CustomComponents) {
						if (!cc.Exist(m_SelectedEntity.id)) continue;
						ImGui::PushID(*id.cbegin());
						const auto &metadata = m_CustomComponentsMetadata.at(id);
						if (m_CustomComponentsImGui.contains(id)) {
							m_CustomComponentsImGui.at(id)(cc.GetView(m_SelectedEntity.id));
						}
						else {
							ImGui::SeparatorText(metadata.name.c_str());
						}
						if (ImGui::Button("Remove")) {
							cc.Remove(m_SelectedEntity.id);
						}
						ImGui::PopID();
					}

					if (ImGui::BeginPopup("add_component_popup")) {
						ImGui::SeparatorText("Add Components");

						for (const auto &[id, cc]: m_CustomComponents) {
							if (cc.Exist(m_SelectedEntity.id)) continue;
							ImGui::PushID(*id.cbegin());
							const auto &metadata = m_CustomComponentsMetadata.at(id);
							if (ImGui::Selectable(metadata.name.c_str())) {
								AddComponent(m_SelectedEntity.id, id);
							}
							ImGui::PopID();
						}
						ImGui::EndPopup();
					}

					ImGui::SeparatorText("Hierarchy");
					{
						if (ImGui::Button("Select Parent")) {
							m_ChosenParent = m_SelectedEntity;
						}

						ImGui::BeginDisabled(!m_Parents.Exist(m_SelectedEntity.id));
						if (ImGui::Button("Move To Root")) {
							MoveToRoot(m_SelectedEntity);
						}
						ImGui::EndDisabled();

						const uint32_t step = 1;
						const uint32_t step_fast = 100;
						ImGui::InputScalar("Parent", ImGuiDataType_U32, &m_ChosenParent.id, &step, &step_fast, "%u");
						ImGui::BeginDisabled(m_ChosenParent == m_SelectedEntity || !m_ChosenParent.IsValid() || !Exist(m_ChosenParent));
						if (ImGui::Button("Set as child")) {
							AddToChild(m_ChosenParent, m_SelectedEntity);
						}
						ImGui::EndDisabled();

						if (ImGui::Button("Delete")) {
							DestroyEntity(m_SelectedEntity);
							m_SelectedEntity = EntityID::NullID;
						}
					}
					ImGui::Separator();
				}
			}
			ImGui::End();
		}
#endif
	}
	void Scene::DrawChildren(EntityID entity) {
#ifdef MGN_IMGUI
		auto it = BeginChild(entity);
		auto end = EndChild();

		while (it != end) {
			EntityID child = it.GetCurrentChild();
			const bool hasChildren = m_Children.Exist(child.id);
			const auto id = child.string();
			uint32_t flags = ImGuiTreeNodeFlags_None;
			if (child == m_SelectedEntity) flags |= ImGuiTreeNodeFlags_Selected;
			if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf;
			if (ImGui::TreeNodeEx(id.c_str(), flags, m_Names.Get(child.id).c_str())) {
				if (ImGui::IsItemFocused())
					m_SelectedEntity = child;
				if (hasChildren) DrawChildren(child);
				ImGui::TreePop();
			}
			++it;
		}
#endif
	}
	void Scene::ForEach(std::function<void(Scene *scene, EntityID entity)> func) {
		for (uint32_t i = 0; i < m_SparseEntities.Count(); ++i) {
			const EntityID id{m_SparseEntities.GetID(i)};
			func(this, id);
		}
	}

	EntityID Scene::Find(const std::function<bool(Scene *, EntityID id)> &func) {
		for (uint32_t i = 0; i < m_SparseEntities.Count(); ++i) {
			const EntityID id{m_SparseEntities.GetID(i)};
			if (func(this, id)) {
				return id;
			}
		}
		return EntityID::NullID;
	}

	void Scene::ForEach(std::function<Buffer(ConstBufferView parentData, Scene *scene, EntityID entity)> func) {
		const auto beg = m_Roots.cbegin();
		const auto end = m_Roots.cend();

		const Buffer rootData{};

		for (auto it = beg; it != end; ++it) {
			const EntityID rootId = *it;
			const Buffer root = func(ConstBufferView{rootData.Get(), rootData.Size()}, this, rootId);
			ForEach(rootId, ConstBufferView{root.Get(), 0, root.Size()}, func);
		}
	}
	void Scene::ForEach(ConstBufferView rootData, std::function<Buffer(ConstBufferView parentData, Scene *scene, EntityID entity)> func) {
		const auto beg = m_Roots.cbegin();
		const auto end = m_Roots.cend();

		for (auto it = beg; it != end; ++it) {
			EntityID rootId = *it;
			Buffer root = func(rootData, this, rootId);
			ForEach(rootId, ConstBufferView{root.Get(), 0, root.Size()}, func);
		}
	}
	void Scene::ForEachWithComponent(UUID id, std::function<void(Scene *scene, EntityID entityId, BufferView component)> func) {
		if (!m_CustomComponents.contains(id)) return;

		auto &components = m_CustomComponents.at(id);

		auto beg = components.begin();
		auto end = components.begin();
		for (auto it = beg; it != end; ++it) {
			EntityID entity{it.GetID()};
			BufferView bv = it.GetView();
			func(this, entity, bv);
		}
	}
	void Scene::ForEachWithComponent(UUID id, std::function<void(const Scene *scene, EntityID entityId, ConstBufferView component)> func) const {
		if (!m_CustomComponents.contains(id)) return;

		auto &components = m_CustomComponents.at(id);

		auto beg = components.cbegin();
		auto end = components.cbegin();
		for (auto it = beg; it != end; ++it) {
			EntityID entity{it.GetID()};
			ConstBufferView bv = it.GetConstView();
			func(this, entity, bv);
		}
	}
	uint64_t Scene::CountComponents(UUID componentID) const {
		if (!m_CustomComponents.contains(componentID)) return 0;

		const auto &cc = m_CustomComponents.at(componentID);
		return cc.Count();
	}

	void Scene::ForEach(EntityID entity, ConstBufferView associatedData, const std::function<Buffer(ConstBufferView parentData, Scene *scene, EntityID entity)> &func) {
		const auto beg = BeginChild(entity);
		const auto end = EndChild();
		for (auto it = beg; it != end; ++it) {
			const EntityID childId = it.GetCurrentChild();
			const Buffer childData = func(associatedData, this, childId);
			ForEach(childId, ConstBufferView{childData.Get(), 0, childData.Size()}, func);
		}
	}

	// Component management
	UUID Scene::RegisterType(std::string name, const uint64_t size, void (*constructor)(void *, uint32_t), void (*destructor)(void *, uint32_t), void (*copy_constructor)(void *, uint32_t, ConstBufferView view)) {
		UUID id{};
		RegisterType(std::move(name), id, size, constructor, destructor, copy_constructor);
		return id;
	}
	void Scene::RegisterImGui(const UUID componentId, ImGuiFunction func) {
		m_CustomComponentsImGui[componentId] = std::move(func);
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
			MGN_CORE_ERROR("The component {} doesn't exist.", componentId.string());
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


	BufferView Scene::TryGetComponent(const EntityID entityId, const UUID componentId) {
		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
			return BufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);

		if (components.Exist(entityId.id)) {
			return BufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

		return {};
	}

	ConstBufferView Scene::TryGetComponent(const EntityID entityId, const UUID componentId) const {
		if (!m_CustomComponents.contains(componentId)) {
			MGN_CORE_ERROR("The component id {} doesn't exist.", componentId.string());
			return ConstBufferView{};
		}

		auto &components = m_CustomComponents.at(componentId);

		if (components.Exist(entityId.id)) {
			return ConstBufferView{components.Get(entityId.id), 0, components.GetDataSize()};
		}

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
		if (!m_CustomComponents.contains(componentId)) {
			return false;
		}
		auto &components = m_CustomComponents.at(componentId);
		return components.Exist(entityId.id);
	}

	bool Scene::RemoveComponent(EntityID entityId, UUID componentId) {
		if (!m_CustomComponents.contains(componentId)) {
			return false;
		}
		auto &components = m_CustomComponents.at(componentId);
		components.Remove(entityId.id);
		return true;
	}
} // namespace Imagine
