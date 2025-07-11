//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"

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

	struct Physicalisable {
	public:
		// Getters
		[[nodiscard]] Vec3 GetLinearVelocity() const {return LinearVelocity;}
		[[nodiscard]] Vec3 GetRadAngularVelocity() const {return AngularVelocity * Math::DegToRad;}
		[[nodiscard]] Vec3 GetDegAngularVelocity() const {return AngularVelocity;}
		[[nodiscard]] Real GetFriction() const {return Friction;}
		[[nodiscard]] Real GetGravityFactor() const {return GravityFactor;}
		[[nodiscard]] bool GetIsAwake() const {return IsAwake;}
		[[nodiscard]] JPH::EActivation GetActivation() const {return IsAwake ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;}

		// Setters
		void SetLinearVelocity(const Vec3 linearVelocity) {LinearVelocity = linearVelocity;}
		void SetDegAngularVelocity(const Vec3 angularVelocity) {AngularVelocity = angularVelocity;}
		void SetRadAngularVelocity(const Vec3 angularVelocity) {AngularVelocity = angularVelocity * Math::RadToDeg;}
		void SetFriction(const Real friction) {Friction = friction;}
		void SetGravityFactor(const Real gravityFactor) {GravityFactor = gravityFactor;}
		void SetIsAwake(const bool isAwake) {IsAwake = isAwake;}
	public:
		RigidbodyType RBType{RB_Dynamic};
		Vec3 LinearVelocity{0};
		Vec3 AngularVelocity{0};
		Real Friction{0.5};
		Real GravityFactor{1.0};
		bool IsAwake{true};
	};

	struct RigibodyIDComponent
	{
		JPH::BodyID BodyID;
	};

	struct RigidbodyDirty
	{
	};
	namespace ThirdParty::ImGuiLib {
		template<>
		inline bool RenderData<Physicalisable>(const char* label, Physicalisable* data) {
			bool changed = false;
#ifdef MGN_IMGUI
			ImGui::PushID(label);
			ImGui::SeparatorText(label);
			changed |= ImGuiLib::DragReal3("Velocity", Math::ValuePtr(data->LinearVelocity));
			changed |= ImGuiLib::DragReal3("Angular Velocity", Math::ValuePtr(data->AngularVelocity));
			// changed |= ImGuiLib::DragReal("Mass", &data->Mass);
			ImGui::PopID();
#endif
			return changed;
		}
	}

} // namespace Imagine
