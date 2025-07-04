//
// Created by ianpo on 13/06/2025.
//

#pragma once

#include <vulkan/vulkan.h>
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/GPU/GPUMaterial.hpp"
#include "Imagine/Rendering/GPU/GPUMaterialInstance.hpp"
#include "Imagine/Rendering/MaterialComponents.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/VulkanDeleter.hpp"
#include "Imagine/Vulkan/VulkanImage.hpp"
#include "Imagine/Vulkan/VulkanTypes.hpp"

namespace Imagine::Vulkan {

	class VulkanRenderer;

	struct VulkanMaterialPipeline {
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	struct VulkanSetLayout {
		std::vector<Core::MaterialBlock> bindings;
	};

	struct VulkanMaterial : public Core::GPUMaterial {
		virtual uint64_t GetID() override;
		VulkanMaterialPipeline pipeline;
		std::vector<VulkanSetLayout> materialLayoutsDescriptions;
		std::vector<VkDescriptorSetLayout> materialLayouts;

		std::vector<Core::MaterialBlock> pushConstantsDescription;
		std::vector<VkPushConstantRange> pushConstants;
	};

	struct VulkanMaterialInstance : public Core::GPUMaterialInstance {
	private:
		static inline uint64_t s_id;
		uint64_t id;
	public:
		using VulkanBinding = std::variant<std::monostate, AllocatedImage, AllocatedBuffer>;
		using VulkanBindingContainer = std::vector<VulkanBinding>;
	public:
		VulkanMaterialInstance();
		virtual ~VulkanMaterialInstance();

		virtual uint64_t GetID() override;

		Core::Weak<VulkanMaterial> material;
		std::vector<VulkanBindingContainer> materialSetVulkanData;
		std::vector<VkDescriptorSet> materialSets;
		Core::MaterialPass passType;
		Core::Ref<Deleter> deleter;
	};

	struct GLTFMetallicRoughness {
		Core::Ref<VulkanMaterial> opaque;
		Core::Ref<VulkanMaterial> transparent;
		// VulkanMaterialPipeline opaquePipeline;
		// VulkanMaterialPipeline transparentPipeline;
		// VkDescriptorSetLayout materialLayout;
		Core::Topology topology = Core::Topology::Triangle;

		// Our target for Uniform Buffers is a 256 bytes alignments
		struct alignas(256) MaterialConstants {
			glm::vec4 colorFactors;
			glm::vec4 metal_factor;
			glm::vec4 rough_factor;
		};

		struct MaterialResources {
			AllocatedImage colorImage;
			VkSampler colorSampler;
			AllocatedImage metalImage;
			VkSampler metalSampler;
			AllocatedImage roughImage;
			VkSampler roughSampler;
			AllocatedImage normalImage;
			VkSampler normalSampler;
			AllocatedImage aoImage;
			VkSampler aoSampler;
			VkBuffer dataBuffer;
			uint32_t dataBufferOffset;
		};

		DescriptorWriter writer;

		void BuildPipeline(VulkanRenderer* renderer);
		void ClearResources(VkDevice device);

		VulkanMaterialInstance WriteMaterial(VkDevice device, Core::MaterialPass pass, const MaterialResources &resources, DescriptorAllocatorGrowable &descriptorAllocator);
	};
};

