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
#include "Relationship.hpp"

namespace Imagine::Core {


	class Scene {
	public:
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
		Scene(const Scene& o) = default;
		/// The copy assignment WILL keep the same ID. Be mindful.
		Scene& operator=(const Scene& s) = default;

		Scene(Scene&& o) noexcept;
		Scene& operator=(Scene&& s) noexcept;

		void swap(Scene& s) noexcept;

	public:
		/// Create a copy of the scene with a different ID.
		Scene Duplicate() const;
	public:
		// CRD (CRUD without the update)
		EntityID CreateEntity();
		EntityID CreateEntity(EntityID parentId);
		Entity &GetEntity(EntityID id);
		const Entity &GetEntity(EntityID id) const;
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

		private:
			Scene *scene{nullptr};
			EntityID current{EntityID::NullID};

		public:
			[[nodiscard]] bool IsValid() const { return scene && current != EntityID::NullID; }
			[[nodiscard]] explicit operator bool() const { return IsValid(); }

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
		};

		RelationshipIterator BeginRelationship(EntityID);
		RelationshipIterator EndRelationship();

		// Relationship management
		void SetParent(EntityID entity, EntityID parent);
		void AddToChild(EntityID entity, EntityID child);
	private:
		void RemoveParent(EntityID child);
		void AddChild(EntityID parent, EntityID orphan);

	public:
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

	private:
		/// Iterate on all the children with a function.
		/// The function will not be called on the selected entity but only the children. (And so recursively)
		/// The function assume the data should be the result of the function 'func' and pass as parameter.
		template<typename T>
		void ForEach(EntityID entity, const T &associatedData, const std::function<T(const T &data, Scene *scene, EntityID entity)> &func);

	public:
		// Component Handling
		void RegisterType(std::string name, UUID componentId, uint64_t size, void (*constructor)(void *, uint32_t) = nullptr, void (*destructor)(void *, uint32_t) = nullptr, void (*copy_constructor)(void *, uint32_t, ConstBufferView view) = nullptr);
		UUID RegisterType(std::string name, uint64_t size, void (*constructor)(void *, uint32_t) = nullptr, void (*destructor)(void *, uint32_t) = nullptr, void (*copy_constructor)(void *, uint32_t, ConstBufferView view) = nullptr);


		template<typename T>
		void RegisterType() {
			RegisterType(
					typeid(T).name(),
					UUID::FromType<T>(),
					sizeof(T),
					[](void *data, uint32_t size) { new (data) T(); },
					[](void *data, uint32_t size) { reinterpret_cast<T *>(data)->~T(); },
					[](void *data, uint32_t size, ConstBufferView view) { new (data) T(view.template As<T>()); });
		}

		BufferView AddComponent(EntityID entityId, UUID componentId);
		BufferView AddComponent(EntityID entityId, UUID componentId, ConstBufferView view);

		BufferView GetComponent(EntityID entityId, UUID componentId);
		ConstBufferView GetComponent(EntityID entityId, UUID componentId) const;

		BufferView TryGetComponent(EntityID entityId, UUID componentId);
		ConstBufferView TryGetComponent(EntityID entityId, UUID componentId) const;

		BufferView GetOrAddComponent(EntityID entityId, UUID componentId);

		bool HasComponent(EntityID entityId, UUID componentId) const;

		template<typename T>
		bool HasComponent(EntityID entityId) const {
			return HasComponent(entityId, UUID::FromType<T>());
		}


		template<typename T>
		T *AddComponent(const EntityID entityId) {
			auto view = AddComponent(entityId, UUID::FromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T, typename... Args>
		T *AddComponent(const EntityID entityId, Args &&...args) {
			T other(std::forward<Args>(args)...);
			const ConstBufferView otherView = {(const void *) &other, 0, sizeof(T)};
			auto view = AddComponent(entityId, UUID::FromType<T>(), otherView);
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		T *GetComponent(const EntityID entityId) {
			auto view = GetComponent(entityId, UUID::FromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		const T *GetComponent(const EntityID entityId) const {
			auto view = GetComponent(entityId, UUID::FromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		T *TryGetComponent(const EntityID entityId) {
			auto view = TryGetComponent(entityId, UUID::FromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		const T *TryGetComponent(const EntityID entityId) const {
			auto view = TryGetComponent(entityId, UUID::FromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		template<typename T>
		T *GetOrAddComponent(const EntityID entityId) {
			auto view = GetOrAddComponent(entityId, UUID::FromType<T>());
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
		UUID GetID() const {return ID;}
	private:
		UUID ID{};
	private:
		AutoIdSparseSet<Entity, uint32_t> m_SparseEntities;
		std::unordered_map<UUID, RawSparseSet<uint32_t>> m_CustomComponents;
		std::unordered_map<UUID, Metadata> m_CustomComponentsMetadata;

		std::unordered_set<EntityID> m_Roots;
		SparseSet<Parent, uint32_t> m_Parents;
		SparseSet<Child, uint32_t> m_Children;
		SparseSet<Sibling, uint32_t> m_Siblings;
		SparseSet<std::string, uint32_t> m_Names;
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
	void Scene::ForEach(EntityID entity, const T &associatedData, const std::function<T(const T &parentData, Scene *scene, EntityID entity)> &func) {
		const auto beg = BeginChild(entity);
		const auto end = EndChild();
		for (auto it = beg; it != end; ++it) {
			EntityID childId = it.GetCurrentChild();
			T childData = func(associatedData, this, childId);
			ForEach<T>(childId, childData, func);
		}
	}
} // namespace Imagine::Core
