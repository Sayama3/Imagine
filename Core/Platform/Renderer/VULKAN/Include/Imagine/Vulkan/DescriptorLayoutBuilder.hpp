//
// Created by ianpo on 27/05/2025.
//

#pragma once

#include "Imagine/Vulkan/Vulkan.hpp"

namespace Imagine::Vulkan {

	// TODO: Add support for per-binding stage flags.
	struct DescriptorLayoutBuilder {
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		void AddBinding(uint32_t binding, VkDescriptorType type);
		void Clear();
		VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
	};

} // Vulkan
// Imagine
