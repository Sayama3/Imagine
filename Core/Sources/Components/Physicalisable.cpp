//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Components/Physicalisable.hpp"

#include "Imagine/Layers/PhysicsLayer.hpp"
#include "Imagine/Physics/ObjectLayerPairFilter.hpp"
#include "Imagine/ThirdParty/JoltPhysics.hpp"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"

namespace Imagine::ColliderShapes
{
	const JPH::Shape* CreateShape(Box box)
	{
		return new JPH::BoxShape(JPH::Vec3Arg{box.Size.x * Real(0.5),box.Size.y * Real(0.5),box.Size.z * Real(0.5)});
	}
	const JPH::Shape* CreateShape(Sphere sphere)
	{
		return new JPH::SphereShape(sphere.Radius);
	}
	const JPH::Shape* CreateShape(Capsule capsule)
	{
		auto cylinderHeight = Math::Max(capsule.Height - (2*capsule.Radius), Real(0));
		if(cylinderHeight >= REAL_EPSILON)
			return new JPH::CapsuleShape(cylinderHeight * Real(0.5), capsule.Radius);
		else
			return new JPH::SphereShape(capsule.Radius);
	}
	const JPH::Shape* CreateShape(Cylinder cylinder)
	{
		return new JPH::CylinderShape(cylinder.Height * 0.5, cylinder.Radius);
	}
}

namespace Imagine {

	Physicalisable::~Physicalisable() {
		if (!BodyID.IsInvalid()) {
			PhysicsLayer::PushDeletion(BodyID);
		}
	}

	JPH::EActivation Physicalisable::GetActivation() const { return IsAwake ? JPH::EActivation::Activate : JPH::EActivation::DontActivate; }
	JPH::ObjectLayer Physicalisable::GetLayer() const {
		switch (RBType) {
			case RB_Static:
				return PhysicalLayers::NON_MOVING;
			case RB_Dynamic:
				return PhysicalLayers::MOVING;
			case RB_Kinematic:
				return PhysicalLayers::MOVING;
		}
		MGN_CORE_ASSERT(false, "The Rigidbody type {} is unknown.", (uint8_t) RBType);
		return PhysicalLayers::NON_MOVING;
	}
	JPH::EMotionType Physicalisable::GetMotionType() const {
		switch (RBType) {
			case RB_Static:
				return JPH::EMotionType::Static;
			case RB_Dynamic:
				return JPH::EMotionType::Dynamic;
			case RB_Kinematic:
				return JPH::EMotionType::Kinematic;
		}
		MGN_CORE_ASSERT(false, "The Rigidbody type {} is unknown.", (uint8_t) RBType);
		return JPH::EMotionType::Static;
	}

	const JPH::Shape *Physicalisable::GetShape() {
		return std::visit([](auto&& args){return ColliderShapes::CreateShape(args);}, Shape);
	}
} // namespace Imagine
