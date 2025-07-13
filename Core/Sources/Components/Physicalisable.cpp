//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Components/Physicalisable.hpp"

#include "Imagine/Assets/AssetManager.hpp"
#include "Imagine/Assets/AssetField.hpp"

#include "Imagine/Layers/PhysicsLayer.hpp"
#include "Imagine/Physics/ObjectLayerPairFilter.hpp"
#include "Imagine/Physics/PhysicsTypeHelpers.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/ThirdParty/JoltPhysics.hpp"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
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
	const JPH::Shape *CreateShape(Cylinder cylinder) {
		return new JPH::CylinderShape(cylinder.Height * 0.5, cylinder.Radius);
	}

	// const JPH::Shape *CreateShape(Mesh m) {
	// 	AssetField<CPUMesh> meshAsset {m.handle};
	// 	if (!meshAsset.IsValid()) {
	// 		return nullptr;
	// 	}
	// 	Ref<CPUMesh> mesh = meshAsset.GetAsset();
	// 	if (!mesh) {
	// 		return nullptr;
	// 	}
	//
	// 	JPH::VertexList inVertices{mesh->Vertices.size()};
	// 	for (int i = 0; i < mesh->Vertices.size(); ++i) {
	// 		inVertices[i].x = mesh->Vertices[i].position.x;
	// 		inVertices[i].y = mesh->Vertices[i].position.y;
	// 		inVertices[i].z = mesh->Vertices[i].position.z;
	// 	}
	//
	// 	const uint32_t trCount = mesh->Lods.back().count / 3;
	// 	JPH::IndexedTriangleList inTriangles{trCount};
	//
	// 	uint32_t offset = mesh->Lods.back().index;
	// 	for (uint32_t iTr = 0; iTr < trCount; ++iTr) {
	// 		const uint32_t i0 = offset + iTr * 3 + 0;
	// 		const uint32_t i1 = offset + iTr * 3 + 1;
	// 		const uint32_t i2 = offset + iTr * 3 + 2;
	// 		inTriangles[iTr].mIdx[0] = mesh->Indices[i0];
	// 		inTriangles[iTr].mIdx[1] = mesh->Indices[i1];
	// 		inTriangles[iTr].mIdx[2] = mesh->Indices[i2];
	// 	}
	//
	// 	JPH::MeshShapeSettings settings{inVertices, inTriangles};
	// 	JPH::Shape::ShapeResult res{};
	// 	JPH::Shape* shape = new JPH::MeshShape(settings, res);
	// 	if (!res.IsValid()) {
	// 		if (res.HasError()) {
	// 			MGN_CORE_ERROR("Error while creating the Physics Mesh Shape\n{}", res.GetError().c_str());
	// 		} else {
	// 			MGN_CORE_ERROR("Error while creating the Physics Mesh Shape");
	// 		}
	// 		delete shape;
	// 		return nullptr;
	// 	}
	//
	// 	return shape;
	// }
} // namespace Imagine::ColliderShapes

namespace Imagine {

	Physicalisable::~Physicalisable() {
		if (!BodyID.IsInvalid()) {
			PhysicsLayer::PushDeletion(BodyID);
		}
	}
	Physicalisable::Physicalisable(const Physicalisable &o) : Shape(o.Shape), RBType(o.RBType), LinearVelocity(o.LinearVelocity), AngularVelocity(o.AngularVelocity), Friction(o.Friction), GravityFactor(o.GravityFactor), IsAwake(o.IsAwake), dirty(true), BodyID(JPH::BodyID::cInvalidBodyID)
	{
	}

	Physicalisable &Physicalisable::operator=(const Physicalisable &o)
	{
		Shape = o.Shape;
		RBType = o.RBType;
		LinearVelocity = o.LinearVelocity;
		AngularVelocity = o.AngularVelocity;
		Friction = o.Friction;
		GravityFactor = o.GravityFactor;
		IsAwake = o.IsAwake;
		dirty = true;
		return *this;
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
		return std::visit([](auto &&args) { return ColliderShapes::CreateShape(args); }, Shape);
	}
} // namespace Imagine
