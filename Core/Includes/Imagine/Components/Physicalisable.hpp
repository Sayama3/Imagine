//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Assets/AssetField.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"

#include "Imagine/ThirdParty/JoltPhysics.hpp"

#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine {

	enum RigidbodyType : uint8_t {
		/// (not moving or simulating)
		RB_Static,
		/// (moved by forces) or
		RB_Dynamic,
		/// (moved by velocities only).
		RB_Kinematic,
	};

	static inline constexpr const char* const RigidbodyTypeNames[] = {"Static", "Dynamic", "Kinematic"};

	namespace ColliderShapes {
		struct Box {Vec3 Size {1,1,1};};
		struct Sphere {Real Radius {0.5};};
		struct Capsule {Real Height {1}; Real Radius{0.5};};
		struct Cylinder {Real Height {1}; Real Radius{0.5}; };

		const JPH::Shape* CreateShape(Box box);
		const JPH::Shape* CreateShape(Sphere sphere);
		const JPH::Shape* CreateShape(Capsule capsule);
		const JPH::Shape* CreateShape(Cylinder cylinder);
	}

	struct Physicalisable {
	public:
		using ShapeVariant = std::variant<ColliderShapes::Box,
								  ColliderShapes::Sphere,
								  ColliderShapes::Capsule,
								  ColliderShapes::Cylinder
		>;
		enum Type : int {
			Box,
			Sphere,
			Capsule,
			Cylinder,
		};
		static inline const std::array<std::string, 5> TypeNames {"Box","Sphere","Capsule","Cylinder"};
		static inline constexpr char TypeNamesCombo[] = "Box\0Sphere\0Capsule\0Cylinder";
	public:
		Physicalisable() = default;
		~Physicalisable();
	public:
		// Getters
		[[nodiscard]] Vec3 GetLinearVelocity() const {return LinearVelocity;}
		[[nodiscard]] Vec3 GetRadAngularVelocity() const {return AngularVelocity * Math::DegToRad;}
		[[nodiscard]] Vec3 GetDegAngularVelocity() const {return AngularVelocity;}
		[[nodiscard]] Real GetFriction() const {return Friction;}
		[[nodiscard]] Real GetGravityFactor() const {return GravityFactor;}
		[[nodiscard]] bool GetIsAwake() const {return IsAwake;}
		[[nodiscard]] JPH::EActivation GetActivation() const;

		JPH::ObjectLayer GetLayer() const;

		JPH::EMotionType GetMotionType() const;

		// Setters
		void SetLinearVelocity(const Vec3 linearVelocity) {LinearVelocity = linearVelocity;}
		void SetDegAngularVelocity(const Vec3 angularVelocity) {AngularVelocity = angularVelocity;}
		void SetRadAngularVelocity(const Vec3 angularVelocity) {AngularVelocity = angularVelocity * Math::RadToDeg;}
		void SetFriction(const Real friction) {Friction = friction;}
		void SetGravityFactor(const Real gravityFactor) {GravityFactor = gravityFactor;}
		void SetIsAwake(const bool isAwake) {IsAwake = isAwake;}
	public:
		const JPH::Shape* GetShape();
	public:
		ShapeVariant Shape;
		RigidbodyType RBType{RB_Dynamic};
		Vec3 LinearVelocity{0};
		Vec3 AngularVelocity{0};
		Real Friction{0.5};
		Real GravityFactor{1.0};
		bool IsAwake{true};

		bool dirty = true;
		JPH::BodyID BodyID;
	};

	namespace ThirdParty::ImGuiLib {


		template<>
		inline bool RenderData<Physicalisable::ShapeVariant>(const char* label, Physicalisable::ShapeVariant* pShape) {
			bool changed = false;
#ifdef MGN_IMGUI
			ImGui::PushID(label);

			if (label) ImGui::Text(label);

			Physicalisable::Type shape = static_cast<Physicalisable::Type>(pShape->index());
			if(ImGui::Combo("Collider Type", (int*)&shape, Physicalisable::TypeNamesCombo) && shape != pShape->index())
			{
				changed = true;
				switch (shape) {
					case Physicalisable::Box:
						pShape->emplace<ColliderShapes::Box>();
						break;
					case Physicalisable::Sphere:
						pShape->emplace<ColliderShapes::Sphere>();
						break;
					case Physicalisable::Capsule:
						pShape->emplace<ColliderShapes::Capsule>();
						break;
					case Physicalisable::Cylinder:
						pShape->emplace<ColliderShapes::Cylinder>();
						break;
				}
			}

			switch (shape) {
				case Physicalisable::Box:
				{
					ColliderShapes::Box& shp = std::get<ColliderShapes::Box>(*pShape);
					changed |= ImGuiLib::DragReal3("Size##Box", Math::ValuePtr(shp.Size), 0.1, 0.1, REAL_MAX, "%.1f");
				}
					break;
				case Physicalisable::Sphere:
				{
					ColliderShapes::Sphere& shp = std::get<ColliderShapes::Sphere>(*pShape);
					changed |= ImGuiLib::DragReal("Radius##Sphere", &shp.Radius, 0.1, 0.1, REAL_MAX, "%.1f");
				}
					break;
				case Physicalisable::Capsule:
				{
					ColliderShapes::Capsule& shp = std::get<ColliderShapes::Capsule>(*pShape);
					changed |= ImGuiLib::DragReal("Radius##Capsule", &shp.Radius, 0.1, 0.1, REAL_MAX, "%.1f");
					changed |= ImGuiLib::DragReal("Height##Capsule", &shp.Height, 0.1, 0, REAL_MAX, "%.1f");
				}
					break;
				case Physicalisable::Cylinder:
				{
					ColliderShapes::Cylinder& shp = std::get<ColliderShapes::Cylinder>(*pShape);
					changed |= ImGuiLib::DragReal("Radius##Cylinder", &shp.Radius, 0.1, 0.1, REAL_MAX, "%.1f");
					changed |= ImGuiLib::DragReal("Height##Cylinder", &shp.Height, 0.1, 0.1, REAL_MAX, "%.1f");
				}
					break;
			}
			ImGui::PopID();
#endif
			return changed;

		}

		template<>
		inline bool RenderData<Physicalisable>(const char* label, Physicalisable* data) {
			bool changed = false;
#ifdef MGN_IMGUI
			ImGui::PushID(label);

			if (label) ImGui::SeparatorText(label);
			else ImGui::Separator();

			changed |= ImGuiLib::RenderData("Shape", &data->Shape);
			changed |= ImGui::Combo("Rigidbody Type", (int*)&data->RBType, "Static\0Dynamic\0Kinematic");
			changed |= ImGuiLib::DragReal3("Velocity", Math::ValuePtr(data->LinearVelocity), 0.1, 0, 0, "%.2f");
			changed |= ImGuiLib::DragReal3("Angular Velocity", Math::ValuePtr(data->AngularVelocity), 1, 0,0 , "%.2f");
			changed |= ImGuiLib::SliderReal("Friction", &data->Friction, 0, 1);
			changed |= ImGuiLib::DragReal("Gravity Factor", &data->GravityFactor, 0.1, 0,0 , "%.2f");
			changed |= ImGui::Checkbox("Awake", &data->IsAwake);
			if (changed) data->dirty = true;
			ImGui::PopID();
#endif
			return changed;
		}

	}

} // namespace Imagine
