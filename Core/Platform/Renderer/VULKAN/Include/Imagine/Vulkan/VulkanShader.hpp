//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Rendering/Shader.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

namespace Imagine::Vulkan {

struct VulkanShader final : public Core::Shader {
	VulkanShader() = default;
	virtual ~VulkanShader() = default;
	virtual const char *GetName() const override;

	std::string name{};
	VkPipeline pipeline = nullptr;
	VkPipelineLayout layout = nullptr;
	std::vector<VkPushConstantRange> pushConstants{};
	Core::Buffer pushConstantsData{};
};

} // Vulkan
// Imagine
