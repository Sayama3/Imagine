//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/Rendering/Camera.hpp"


namespace Imagine::Core {
	Camera* Camera::s_MainCamera = new Camera();

	glm::mat4 Camera::GetViewMatrix() const {
		// to create a correct model view, we need to move the world in opposite
		// direction to the camera
		//  so we will create the camera model matrix and invert
		glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), position);
		glm::mat4 cameraRotation = GetRotationMatrix();
		return glm::inverse(cameraTranslation * cameraRotation);
	}

	glm::mat4 Camera::GetRotationMatrix() const {
		// fairly typical FPS style camera. we join the pitch and yaw rotations into
		// the final rotation matrix

		glm::quat pitchRotation = glm::angleAxis(glm::radians(pitch), glm::vec3{1.f, 0.f, 0.f});
		glm::quat yawRotation = glm::angleAxis(glm::radians(yaw), glm::vec3{0.f, 1.f, 0.f});

		return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
	}
	Vec3 Camera::GetForward() const {
		const Mat4 cameraRotation = GetRotationMatrix();
		return Vec3(cameraRotation * Vec4{0,0,1,0});
	}
	void Camera::Update(TimeStep timestep) {
		yaw += yawVelocity * timestep;
		pitch += pitchVelocity * timestep;
		
		const glm::mat4 cameraRotation = GetRotationMatrix();
		position += glm::vec3(cameraRotation * glm::vec4(velocity * (Real)timestep.GetSeconds() * velocityMultiplier, 0.f));

	}

} // namespace Imagine::Core
