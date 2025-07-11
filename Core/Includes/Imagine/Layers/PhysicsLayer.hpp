//
// Created by ianpo on 10/07/2025.
//

#pragma once

#include "Imagine/Events/ApplicationEvent.hpp"
#include "Imagine/Layers/Layer.hpp"
#include "Imagine/ThirdParty/JoltPhysics.hpp"

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Physics/ObjectLayerPairFilter.hpp"
#include "Imagine/Physics/ObjectVsBroadPhaseLayerFilter.hpp"
#include "Imagine/Physics/PhysicsListener.hpp"

namespace Imagine {

	class Scene;

	class PhysicsLayer final : public Layer {
	public:
		PhysicsLayer();
		virtual ~PhysicsLayer() override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &event) override;
	public:
		static bool IsSimulating();

	private:
		void OnUpdate(AppUpdateEvent &event);
		void OnImGui(ImGuiEvent &event);

	private:
		void Update(Scene* scene, TimeStep ts);

	public:
		/// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
		static constexpr uint32_t cMaxBodies = 65536;

		/// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
		static constexpr uint32_t cNumBodyMutexes = 0;

		/// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
		/// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
		/// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
		static constexpr uint32_t cMaxBodyPairs = 65536;

		/// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
		/// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
		static constexpr uint32_t cMaxContactConstraints = 10240;
	public:

		/// We need a temp allocator for temporary allocations during the physics update. We're
		/// pre-allocating 10 MB to avoid having to do allocations during the physics update.
		JPH::TempAllocatorImpl m_TempAllocator{10 * 1024 * 1024};

		/// We need a job system that will execute physics jobs on multiple threads. Typically
		/// you would implement the JobSystem interface yourself and let Jolt Physics run on top
		/// of your own job scheduler. JobSystemThreadPool is an example implementation.
		JPH::JobSystemThreadPool m_JobSystem{JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, static_cast<int>(std::thread::hardware_concurrency() - 1)};
	private:
		Scope<JPH::PhysicsSystem> m_PhysicsSystem;

		/// Create mapping table from object layer to broadphase layer
		/// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		BroadPhaseLayerInterface m_BroadPhaseLayer;

		/// Create class that filters object vs broadphase layers
		/// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		ObjectVsBroadPhaseLayerFilter m_ObjectVsBroadphaseLayerFilter;

		/// Create class that filters object vs object layers
		/// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		ObjectLayerPairFilter m_ObjectVsObjectLayerFilter;

		/// A body activation listener gets notified when bodies activate and go to sleep
		/// Note that this is called from a job so whatever you do here needs to be thread safe.
		/// Registering one is entirely optional.
		LoggerActivationListener m_BodyActivationListener;

		/// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		/// Note that this is called from a job so whatever you do here needs to be thread safe.
		/// Registering one is entirely optional.
		LoggerContactListener m_ContactListener;

		JPH::BodyInterface* m_BodyInterface = nullptr;
	private:
		static inline bool s_Simulate{false};
	};

} // namespace Imagine
