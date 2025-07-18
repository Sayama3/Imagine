//
// Created by ianpo on 31/07/2024.
//

#include "Imagine/Physics/ObjectLayerPairFilter.hpp"
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
// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

namespace Imagine
{
	bool ObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
	{
		switch (inObject1)
		{
			case PhysicalLayers::NON_MOVING:
				return inObject2 == PhysicalLayers::MOVING; // Non moving only collides with moving
			case PhysicalLayers::MOVING:
				return true; // Moving collides with everything
			default:
				MGN_CORE_ASSERT(false, "The layer {} is not valid.", inObject1);
				return false;
		}
	}
}// namespace Imagine