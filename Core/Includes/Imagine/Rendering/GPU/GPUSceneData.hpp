//
// Created by ianpo on 09/07/2025.
//


#pragma once

#include "Imagine/Math/Core.hpp"
namespace Imagine::Core {
	struct GPUSceneData {
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 viewproj;
		glm::vec4 cameraPosition;
		glm::vec4 ambientColor;
		glm::vec4 sunlightDirection; // w for sun power
		glm::vec4 sunlightColor;
	};
}