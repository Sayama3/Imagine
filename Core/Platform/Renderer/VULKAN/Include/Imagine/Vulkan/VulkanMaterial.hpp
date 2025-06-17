//
// Created by ianpo on 13/06/2025.
//

#pragma once

#include <vulkan/vulkan.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/Material.hpp"
#include "Imagine/Vulkan/AllocatedImage.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"

namespace Imagine::Vulkan {

	class VulkanRenderer;

	struct VulkanMaterialPipeline {
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	struct VulkanMaterialInstance : public Core::MaterialInstance {
		VulkanMaterialInstance() = default;
		virtual ~VulkanMaterialInstance() = default;

		VulkanMaterialPipeline *pipeline;
		VkDescriptorSet materialSet;
	};

	struct GLTFMetallicRoughness {
		VulkanMaterialPipeline opaquePipeline;
		VulkanMaterialPipeline transparentPipeline;

		VkDescriptorSetLayout materialLayout;

		// Our target for Uniform Buffers is a 256 bytes alignments
		struct alignas(256) MaterialConstants {
			glm::vec4 colorFactors;
			glm::vec4 metal_rough_factors;
		};

		struct MaterialResources {
			AllocatedImage colorImage;
			VkSampler colorSampler;
			AllocatedImage metalRoughImage;
			VkSampler metalRoughSampler;
			VkBuffer dataBuffer;
			uint32_t dataBufferOffset;
		};

		DescriptorWriter writer;

		void BuildPipeline(VulkanRenderer* renderer);
		void ClearResources(VkDevice device);

		VulkanMaterialInstance WriteMaterial(VkDevice device, Core::MaterialPass pass, const MaterialResources &resources, DescriptorAllocatorGrowable &descriptorAllocator);
	};
};

