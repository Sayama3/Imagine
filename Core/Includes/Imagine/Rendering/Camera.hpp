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
		glm::vec3 velocity{0};
		glm::vec3 position{0,2,-2};
		// vertical rotation
		float pitch { 45.f };
		// horizontal rotation
		float yaw { 0.f };

		float pitchVelocity { 0.f };
		float yawVelocity { 0.f };

		float velocityMultiplier = 2.f;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetRotationMatrix() const;
		Vec3 GetForward() const;

		void Update(TimeStep timestep);

	};

} // namespace Imagine
