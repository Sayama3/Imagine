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

namespace Imagine::Core {

	class Scene;

	class PhysicsLayer final : public Layer {
	public:
		PhysicsLayer() = default;
		virtual ~PhysicsLayer() override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &event) override;
	private:
		bool OnUpdate(AppUpdateEvent &event);
		bool OnImGui(ImGuiEvent &event);

	private:
		void Update(Scene* scene, TimeStep dt);

	public:
	std::vector<Weak<Scene>> scenes;

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

	};

} // namespace Imagine::Core
