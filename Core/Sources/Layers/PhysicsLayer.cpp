//
// Created by ianpo on 10/07/2025.
//

#include "Imagine/Layers/PhysicsLayer.hpp"
// Jolt includes
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

namespace Imagine::Core {

	PhysicsLayer::~PhysicsLayer() {
	}
	void PhysicsLayer::OnAttach() {

		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		// It is not directly used in this example but still required.
		JPH::Factory::sInstance = new JPH::Factory();

		// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
		// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
		// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
		JPH::RegisterTypes();
	}
	void PhysicsLayer::OnDetach() {
		// Unregisters all types with the factory and cleans up the default material
		JPH::UnregisterTypes();

		// Destroy the factory
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	void PhysicsLayer::OnEvent(Event &event) {
		EventDispatcher dispatch(event);
		dispatch.Dispatch<AppUpdateEvent>(MGN_BIND_EVENT_FN(OnUpdate));
	}

	bool PhysicsLayer::OnUpdate(AppUpdateEvent &event) {



		return false;
	}
} // namespace Imagine::Core
