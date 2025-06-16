//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Entity.hpp"
#include "Relationship.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/RawSparseSet.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core {


	class Scene {
	public:
		static constexpr uint64_t c_EntityPrepareCount = 1024;

	public:
		Scene();
		~Scene();

	public:
		// CRD (CRUD without the update)
		EntityID CreateEntity();
		Entity &GetEntity(EntityID id);
		const Entity &GetEntity(EntityID id) const;
		void DestroyEntity(EntityID id);
		void Clear();

	public:

		class RelationshipIterator {
		public:
			RelationshipIterator() = default;
			~RelationshipIterator() = default;
			RelationshipIterator(const RelationshipIterator &o);
			RelationshipIterator &operator=(const RelationshipIterator &o);
			RelationshipIterator(Scene* scene, EntityID id);

		private:
			Scene* scene{nullptr};
			EntityID current{EntityID::NullID};
		public:
			[[nodiscard]] bool IsValid() const {return scene && current != EntityID::NullID;}
			[[nodiscard]] explicit operator bool() const {return IsValid(); }

			[[nodiscard]] Entity& Get();
			[[nodiscard]] const Entity& Get() const;

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

			//TODO: Test see if it works.
			RelationshipIterator& operator ++(int);
		};

		// Relationship management
		void SetParent(EntityID entity, EntityID parent);
		void AddToChild(EntityID entity, EntityID child);

	public:
		// Component Handling
		void AddComponentType(UUID componentId, uint64_t size, void (*constructor)(void *, uint32_t) = nullptr, void (*destructor)(void *, uint32_t) = nullptr, void (*copy_constructor)(void *, uint32_t, ConstBufferView view) = nullptr);
		UUID AddComponentType(uint64_t size, void (*constructor)(void *, uint32_t) = nullptr, void (*destructor)(void *, uint32_t) = nullptr, void (*copy_constructor)(void *, uint32_t, ConstBufferView view) = nullptr);


		template<typename T>
		void AddComponentType() {
			AddComponentType(
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
		BufferView GetOrAddComponent(EntityID entityId, UUID componentId);

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
		T *GetOrAddComponent(const EntityID entityId) {
			auto view = GetOrAddComponent(entityId, UUID::FromType<T>());
			return view.IsValid() ? view.template Get<T>() : nullptr;
		}

		void ForEach(std::function<void(const glm::mat4& worldMat, Entity& entity)>);

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

	private:
		AutoIdSparseSet<Entity, uint32_t> m_SparseEntities;
		std::unordered_map<UUID, RawSparseSet<uint32_t>> m_CustomComponents;

		HeapArray<EntityID> m_Roots;
		SparseSet<Parent, uint32_t> m_Parents;
		SparseSet<Child, uint32_t> m_Children;
		SparseSet<Sibling, uint32_t> m_Siblings;
	};
} // namespace Imagine::Core
