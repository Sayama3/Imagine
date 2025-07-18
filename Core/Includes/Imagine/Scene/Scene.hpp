//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Entity.hpp"
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/RawSparseSet.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Core/TypeHelper.hpp"
#include "Imagine/Core/UUID.hpp"
#include "Imagine/Math/Transform.hpp"
#include "Imagine/Rendering/Light.hpp"
#include "Relationship.hpp"

namespace Imagine {

	class SceneSerializer;
	class Scene final : public Asset {
		friend class SceneSerializer;

	public:
		MGN_IMPLEMENT_ASSET(AssetType::Scene);

	public:
		using ImGuiFunction = std::function<bool(BufferView)>;
		static constexpr uint64_t c_EntityPrepareCount = 1024;
		using Ref = std::shared_ptr<Scene>;

		/// Struct holding the component metadata.
		struct Metadata {
			std::string name;
			UUID id;
			uint64_t size;
			bool hasConstructor;
			bool hasDestructor;
			bool hasCopyConstructor;
		};

	public:
		Scene();
		~Scene();

		/// The copy constructor WILL keep the same ID. Be mindful.
		Scene(const Scene &o) = default;

		/// The copy assignment WILL keep the same ID. Be mindful.
		Scene &operator=(const Scene &s) = default;

	public:
		/// Create a copy of the scene with a different ID.
		Scene Duplicate() const;

	public:
		// CRD (CRUD without the update)
		EntityID CreateEntity();
		EntityID CreateEntity(EntityID parentId);
		Entity &GetEntity(EntityID id);
		const Entity &GetEntity(EntityID id) const;
		bool Exist(EntityID id) const;
		void DestroyEntity(EntityID id);
		void Clear();

		[[nodiscard]] std::string GetName(EntityID entityId) const;
		void SetName(EntityID entityId, std::string name);

	public:
		// Iterator used to iterate on all the child of an entity.
		class ChildIterator {
		public:
			ChildIterator() = default;
			~ChildIterator() = default;
			ChildIterator(const ChildIterator &) = default;
			ChildIterator &operator=(const ChildIterator &) = default;
			ChildIterator(Scene *scene, EntityID parent);

		public:
			[[nodiscard]] ChildIterator Next() const;
			[[nodiscard]] ChildIterator Previous() const;
			[[nodiscard]] EntityID GetParent() const;
			[[nodiscard]] EntityID GetCurrentChild() const { return currentChild; }
			[[nodiscard]] Scene *GetScene() const { return scene; }

		public:
			ChildIterator operator++(int);
			ChildIterator &operator++();
			ChildIterator operator--(int);
			ChildIterator &operator--();

			bool operator==(const ChildIterator &o) const { return scene == o.scene && currentChild == o.currentChild; }
			bool operator!=(const ChildIterator &o) const { return !(*this == o); }
			auto operator<=>(const ChildIterator &o) const { return currentChild <=> o.currentChild; }

		private:
			Scene *scene{nullptr};
			EntityID currentChild{EntityID::NullID};
		};

		ChildIterator BeginChild(EntityID parent);
		ChildIterator EndChild();

		// Iterator used to iterate on all the member of a hierarchy, parent to children, by going only once on each entity.
		class RelationshipIterator {
		public:
			RelationshipIterator() = default;
			~RelationshipIterator() = default;
			RelationshipIterator(const RelationshipIterator &o);
			RelationshipIterator &operator=(const RelationshipIterator &o);
			RelationshipIterator(Scene *scene, EntityID id);
			RelationshipIterator(Scene *scene, EntityID id, EntityID rootParent);

		private:
			Scene *scene{nullptr};
			EntityID current{EntityID::NullID};
			EntityID rootParent{EntityID::NullID};

		public:
			[[nodiscard]] bool IsValid() const { return scene && current != EntityID::NullID; }
			[[nodiscard]] explicit operator bool() const { return IsValid(); }

			EntityID GetID() const { return current; }

			[[nodiscard]] Entity &Get();
			[[nodiscard]] const Entity &Get() const;

			[[nodiscard]] bool IsRoot() const;
			[[nodiscard]] bool HasChildren() const;
			[[nodiscard]] bool HasParent() const;

			/// This method will calculate the same way an linked-list works all the children this entity have.
			/// @return The number of children this entity have.
			[[nodiscard]] uint64_t CountChildren() const;

			[[nodiscard]] RelationshipIterator PreviousSibling() const;
			[[nodiscard]] RelationshipIterator NextSibling() const;
			[[nodiscard]] RelationshipIterator Parent() const;
			[[nodiscard]] RelationshipIterator FirstChild() const;

			RelationshipIterator operator++(int);
			RelationshipIterator &operator++();

			bool operator==(const RelationshipIterator &o) const {
				return scene == o.scene && current == o.current;
			}
			bool operator!=(const RelationshipIterator &o) const {
				return !(*this == o);
			}
		};

		RelationshipIterator BeginRelationship(EntityID);
		RelationshipIterator EndRelationship();

		// Relationship management
		void SetParent(EntityID entity, EntityID parent);
		void AddToChild(EntityID entity, EntityID child);

		void MoveToRoot(EntityID entity);

	private:
		void RemoveParent(EntityID child);
		void AddChild(EntityID parent, EntityID orphan);

	public:
		void CacheTransforms();
		Mat4 GetWorldTransform(EntityID id) const;
		TransformR GetTransform(EntityID id) const;

	public:
		void SendImGuiCommands();

	private:
		void DrawChildren(EntityID entity);

	public:
		/// Iterate on all the entity with a function.
		/// Will iterate recursively on all the entity and will pass some data from parent to children.
		/// As recusivity imply, this function is as resource intensive as the hierarchy go down.
		void ForEach(std::function<void(Scene *scene, EntityID entity)> func);

		/// Iterate on all the entity with a function.
		/// Will iterate recursively on all the entity and will pass some data from parent to children.
		/// As recusivity imply, this function is as resource intensive as the hierarchy go down.
		template<typename T>
		void ForEach(std::function<T(const T &parentData, Scene *scene, EntityID entity)>);

		/// Iterate on all the children with a function.
		/// Will iterate recursively on all the entity and will pass some data from parent to children.
		/// As recusivity imply, this function is as resource intensive as the hierarchy go down.
		template<typename T>
		void ForEach(const T &rootData, std::function<T(const T &parentData, Scene *scene, EntityID entity)>);

		template<typename T>
		void ForEachWithComponent(std::function<void(Scene *scene, EntityID entityId, T &component)>);

		template<typename T>
		void ForEachWithComponent(std::function<void(const Scene *scene, EntityID entityId, const T &component)>) const;

		template<typename T>
		[[nodiscard]] uint64_t CountComponents() const {
			return CountComponents(UUIDFromType<T>());
		}

		EntityID Find(const std::function<bool(Scene*, EntityID id)>& func);
	public:
		void ForEach(std::function<Buffer(ConstBufferView parentData, Scene *scene, EntityID entity)>);
		void ForEach(ConstBufferView rootData, std::function<Buffer(ConstBufferView parentData, Scene *scene, EntityID entity)>);

		void ForEachWithComponent(UUID componentID, std::function<void(Scene *scene, EntityID entityId, BufferView component)>);
		void ForEachWithComponent(UUID componentID, std::function<void(const Scene *scene, EntityID entityId, ConstBufferView component)>) const;
		[[nodiscard]] uint64_t CountComponents(UUID componentID) const;

	private:
		/// Iterate on all the children with a function.
		/// The function will not be called on the selected entity but only the children. (And so recursively)
		/// The function assume the data should be the result of the function 'func' and pass as parameter.
		template<typename T>
		void ForEach(EntityID entity, const T &associatedData, const std::function<T(const T &parentData, Scene *scene, EntityID entity)> &func);

		void ForEach(EntityID entity, ConstBufferView associatedData, const std::function<Buffer(ConstBufferView parentData, Scene *scene, EntityID entity)> &func);

	public:
		// Component Handling
		void RegisterType(std::string name, UUID componentId, uint64_t size, void (*constructor)(void *, uint32_t) = nullptr, void (*destructor)(void *, uint32_t) = nullptr, void (*copy_constructor)(void *, uint32_t, ConstBufferView view) = nullptr);
		UUID RegisterType(std::string name, uint64_t size, void (*constructor)(void *, uint32_t) = nullptr, void (*destructor)(void *, uint32_t) = nullptr, void (*copy_constructor)(void *, uint32_t, ConstBufferView view) = nullptr);

		void RegisterImGui(UUID componentId, ImGuiFunction);


		template<typename T>
		void RegisterType() {
			RegisterType(
					NiceTypeName<T>(),
					UUIDFromType<T>(),
					sizeof(T),
					[](void *data, uint32_t size) { new (data) T(); },
					[](void *data, uint32_t size) { reinterpret_cast<T *>(data)->~T(); },
					[](void *data, uint32_t size, ConstBufferView view) { new (data) T(view.template As<T>()); });
			RegisterImGui<T>();
		}

		template<typename T>
		void RegisterImGui() {
			RegisterImGui(UUIDFromType<T>(), [](BufferView view) { return ThirdParty::ImGuiLib::RenderData<T>(NiceTypeName<T>(), view.Get<T>()); });
		}

		BufferView AddComponent(EntityID entityId, UUID componentId);
		BufferView AddComponent(EntityID entityId, UUID componentId, ConstBufferView view);

		BufferView GetComponent(EntityID entityId, UUID componentId);
		ConstBufferView GetComponent(EntityID entityId, UUID componentId) const;

		BufferView TryGetComponent(EntityID entityId, UUID componentId);
		ConstBufferView TryGetComponent(EntityID entityId, UUID componentId) const;

		BufferView GetOrAddComponent(EntityID entityId, UUID componentId);

		[[nodiscard]] bool HasComponent(EntityID entityId, UUID componentId) const;

		bool RemoveComponent(EntityID entityId, UUID componentId);

		template<typename T>
		[[nodiscard]] bool HasComponent(EntityID entityId) const {
			return HasComponent(entityId, UUIDFromType<T>());
		}

		template<typename T>
		bool RemoveComponent(EntityID entityId) {
			return RemoveComponent(entityId, UUIDFromType<T>());
		}


		template<typename T>
		T *AddComponent(const EntityID entityId) {
			auto view = AddComponent(entityId, UUIDFromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T, typename... Args>
		T *AddComponent(const EntityID entityId, Args &&...args) {
			T other(std::forward<Args>(args)...);
			const ConstBufferView otherView = {(const void *) &other, 0, sizeof(T)};
			auto view = AddComponent(entityId, UUIDFromType<T>(), otherView);
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		T *GetComponent(const EntityID entityId) {
			auto view = GetComponent(entityId, UUIDFromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		const T *GetComponent(const EntityID entityId) const {
			auto view = GetComponent(entityId, UUIDFromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		T *TryGetComponent(const EntityID entityId) {
			auto view = TryGetComponent(entityId, UUIDFromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		const T *TryGetComponent(const EntityID entityId) const {
			auto view = TryGetComponent(entityId, UUIDFromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		T *GetOrAddComponent(const EntityID entityId) {
			auto view = GetOrAddComponent(entityId, UUIDFromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		[[nodiscard]] uint32_t Count() const {
			return m_SparseEntities.Count();
		}

		void Reserve(const uint32_t capacity) {
			m_SparseEntities.Reserve(capacity);
			for (auto &[uuid, comps]: m_CustomComponents) {
				comps.Reserve(capacity);
			}
		}

		void Prepare(const uint32_t additional_capacity) {
			m_SparseEntities.Prepare(additional_capacity);
			for (auto &[uuid, comps]: m_CustomComponents) {
				comps.Prepare(additional_capacity);
			}
		}

	public:
		UUID GetID() const { return Handle.GetID(); }

	private:
		AutoIdSparseSet<Entity, uint32_t> m_SparseEntities;
		std::unordered_map<UUID, RawSparseSet<uint32_t>> m_CustomComponents;
		std::unordered_map<UUID, Metadata> m_CustomComponentsMetadata;
		std::unordered_map<UUID, ImGuiFunction> m_CustomComponentsImGui;

		std::unordered_set<EntityID> m_Roots;
		SparseSet<Parent, uint32_t> m_Parents;
		SparseSet<Child, uint32_t> m_Children;
		SparseSet<Sibling, uint32_t> m_Siblings;
		SparseSet<std::string, uint32_t> m_Names;
		SparseSet<TransformR, uint32_t> m_WorldTransform;

	private:
		// Dedicated to ImGui Rendering.
		EntityID m_SelectedEntity;
		EntityID m_ChosenParent;
	};


	template<typename T>
	void Scene::ForEach(std::function<T(const T &worldMat, Scene *scene, EntityID entity)> func) {
		const auto beg = m_Roots.cbegin();
		const auto end = m_Roots.cend();

		T data{};

		for (auto it = beg; it != end; ++it) {
			EntityID rootId = *it;
			T root = func(data, this, rootId);
			ForEach<T>(rootId, root, func);
		}
	}

	template<typename T>
	void Scene::ForEach(const T &rootData, std::function<T(const T &parentData, Scene *scene, EntityID entity)> func) {
		const auto beg = m_Roots.cbegin();
		const auto end = m_Roots.cend();

		for (auto it = beg; it != end; ++it) {
			EntityID rootId = *it;
			T root = func(rootData, this, rootId);
			ForEach<T>(rootId, root, func);
		}
	}

	template<typename T>
	void Scene::ForEachWithComponent(std::function<void(Scene *scene, EntityID entityId, T &component)> func) {
		const auto id = UUIDFromType<T>();
		if (!m_CustomComponents.contains(id)) return;

		auto &components = m_CustomComponents.at(id);

		auto beg = components.begin();
		auto end = components.end();
		for (auto it = beg; it != end; ++it) {
			EntityID entity{it.GetID()};
			BufferView bv = it.GetView();
			func(this, entity, bv.As<T>());
		}
	}

	template<typename T>
	void Scene::ForEachWithComponent(std::function<void(const Scene *scene, EntityID entityId, const T &component)> func) const {
		const auto id = UUIDFromType<T>();
		if (!m_CustomComponents.contains(id)) return;

		auto &components = m_CustomComponents.at(id);

		auto beg = components.cbegin();
		auto end = components.cend();
		for (auto it = beg; it != end; ++it) {
			EntityID entity{it.GetID()};
			ConstBufferView bv = it.GetConstView();
			func(this, entity, bv.As<T>());
		}
	}

	template<typename T>
	void Scene::ForEach(EntityID entity, const T &associatedData, const std::function<T(const T &parentData, Scene *scene, EntityID entity)> &func) {
		const auto beg = BeginChild(entity);
		const auto end = EndChild();
		for (auto it = beg; it != end; ++it) {
			EntityID childId = it.GetCurrentChild();
			T childData = func(associatedData, this, childId);
			ForEach<T>(childId, childData, func);
		}
	}
} // namespace Imagine
