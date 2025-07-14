//
// Created by Iannis on 23/06/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/TimeStep.hpp"

namespace Imagine {

	class Camera {
	public:
		static Camera* s_MainCamera;
	public:
		Vec3 velocity{0};
		Vec3 position{-7, 6, 0};
		// vertical rotation
		float pitch { 30.f };
		// horizontal rotation
		float yaw { 90.f };

		float pitchVelocity { 0.f };
		float yawVelocity { 0.f };

		float velocityMultiplier = 4.f;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetRotationMatrix() const;
		Vec3 GetForward() const;

		void Update(TimeStep timestep);

	};

} // namespace Imagine
