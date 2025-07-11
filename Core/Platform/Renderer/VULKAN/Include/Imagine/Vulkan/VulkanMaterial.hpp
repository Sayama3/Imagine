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
		std::vector<MaterialBlock> bindings;
	};

	struct VulkanMaterial final : public GPUMaterial {
		VulkanMaterial();
		virtual ~VulkanMaterial() override;
		virtual uint64_t GetID() override;
		VulkanMaterialPipeline pipeline;
		std::vector<VulkanSetLayout> materialLayoutsDescriptions;
		std::vector<VkDescriptorSetLayout> materialLayouts;

		std::vector<MaterialBlock> pushConstantsDescription;
		std::vector<VkPushConstantRange> pushConstants;
		bool autoDelete = true;
	};

	struct VulkanMaterialInstance final : public GPUMaterialInstance {
	private:
		static uint64_t s_id;
		uint64_t id;
	public:
		using VulkanBinding = std::variant<std::monostate, AllocatedImage, AllocatedBuffer>;
		using VulkanBindingContainer = std::vector<VulkanBinding>;
	public:
		VulkanMaterialInstance();
		virtual ~VulkanMaterialInstance() override;

		virtual uint64_t GetID() override;

		Weak<VulkanMaterial> material;
		std::vector<VulkanBindingContainer> materialSetVulkanData;
		std::vector<VkDescriptorSet> materialSets;
		MaterialPass passType;
		Ref<Deleter> deleter;
	};

	struct GLTFMetallicRoughness {
		Ref<VulkanMaterial> opaque;
		Ref<VulkanMaterial> transparent;
		// VulkanMaterialPipeline opaquePipeline;
		// VulkanMaterialPipeline transparentPipeline;
		// VkDescriptorSetLayout materialLayout;
		Topology topology = Topology::Triangle;

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

		VulkanMaterialInstance WriteMaterial(VkDevice device, MaterialPass pass, const MaterialResources &resources, DescriptorAllocatorGrowable &descriptorAllocator);
	};
};

