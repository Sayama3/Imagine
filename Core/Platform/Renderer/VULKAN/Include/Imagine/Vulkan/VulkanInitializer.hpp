//
// Created by Iannis on 26/05/2025.
//

#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Scene/Entity.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Vulkan/VulkanTypes.hpp"

namespace Imagine::Vulkan {

	class VulkanRenderer;

	namespace Initializer {
		inline static VkCommandPoolCreateInfo CommandPoolCreateInfo(const uint32_t queueFamilyIndex, const VkCommandPoolCreateFlags flags = 0) {
			VkCommandPoolCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.pNext = nullptr;

			info.queueFamilyIndex = queueFamilyIndex;
			info.flags = flags;
			return info;
		}

		inline static VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, const uint32_t count = 1) {
			VkCommandBufferAllocateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.pNext = nullptr;

			info.commandPool = pool;
			info.commandBufferCount = count;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // TODO: See what the thing is all about with the secondary buffers
			return info;
		}

		inline static VkFenceCreateInfo FenceCreateInfo(const VkFenceCreateFlags flags = 0) {
			VkFenceCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.pNext = nullptr;
			info.flags = flags;

			return info;
		}

		inline static VkSemaphoreCreateInfo SemaphoreCreateInfo(const VkSemaphoreCreateFlags flags = 0) {
			VkSemaphoreCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			info.pNext = nullptr;
			info.flags = flags;
			return info;
		}

		inline static VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferUsageFlags flags = 0) {
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.pNext = nullptr;

			info.pInheritanceInfo = nullptr;
			info.flags = flags;
			return info;
		}

		inline static VkImageSubresourceRange ImageSubresourceRange(const VkImageAspectFlags aspectMask) {
			VkImageSubresourceRange subImage = {};
			subImage.aspectMask = aspectMask;
			subImage.baseMipLevel = 0;
			subImage.levelCount = VK_REMAINING_MIP_LEVELS;
			subImage.baseArrayLayer = 0;
			subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

			return subImage;
		}

		inline static VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) {
			VkSemaphoreSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.semaphore = semaphore;
			submitInfo.stageMask = stageMask;
			submitInfo.deviceIndex = 0;
			submitInfo.value = 1;

			return submitInfo;
		}

		inline static VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd) {
			VkCommandBufferSubmitInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			info.pNext = nullptr;
			info.commandBuffer = cmd;
			info.deviceMask = 0;

			return info;
		}

		inline static VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphoreInfo, VkSemaphoreSubmitInfo *waitSemaphoreInfo) {
			VkSubmitInfo2 info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			info.pNext = nullptr;

			info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
			info.pWaitSemaphoreInfos = waitSemaphoreInfo;

			info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
			info.pSignalSemaphoreInfos = signalSemaphoreInfo;

			info.commandBufferInfoCount = 1;
			info.pCommandBufferInfos = cmd;

			return info;
		}

		inline static VkImageCreateInfo ImageCreateInfo2D(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent) {
			// TODO: Create MipMaps
			VkImageCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.pNext = nullptr;

			info.imageType = VK_IMAGE_TYPE_2D;

			info.format = format;
			info.extent = extent;

			info.mipLevels = 1;
			info.arrayLayers = 1;

			// for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
			info.samples = VK_SAMPLE_COUNT_1_BIT;

			// optimal tiling, which means the image is stored on the best gpu format
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.usage = usageFlags;

			return info;
		}

		inline static VkImageViewCreateInfo ImageViewCreateInfo2D(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags) {
			// build a image-view for the depth image to use for rendering
			VkImageViewCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.pNext = nullptr;

			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.image = image;
			info.format = format;

			// TODO: Create MipMaps
			info.subresourceRange.baseMipLevel = 0;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.baseArrayLayer = 0;
			info.subresourceRange.layerCount = 1;
			info.subresourceRange.aspectMask = aspectFlags;

			return info;
		}

		inline static VkRenderingAttachmentInfo RenderingAttachmentInfo(VkImageView view, VkClearValue *clear, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			VkRenderingAttachmentInfo colorAttachment{};
			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.pNext = nullptr;

			colorAttachment.imageView = view;
			colorAttachment.imageLayout = layout;
			colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			if (clear) {
				colorAttachment.clearValue = *clear;
			}

			return colorAttachment;
		}

		inline static VkRenderingAttachmentInfo DepthAttachmentInfo(VkImageView view, float clearValue = 1.0f, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			VkRenderingAttachmentInfo depthAttachment {};
			depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.pNext = nullptr;

			depthAttachment.imageView = view;
			depthAttachment.imageLayout = layout;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			depthAttachment.clearValue.depthStencil.depth = clearValue;

			return depthAttachment;
		}

		inline static VkRenderingInfo RenderingInfo(VkExtent2D extents, VkRenderingAttachmentInfo *colorAttachement, VkRenderingAttachmentInfo *depthAttachement) {
			VkRenderingInfo renderingInfo = {};
			renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			renderingInfo.pNext = nullptr;

			renderingInfo.flags = 0;
			renderingInfo.renderArea = {VkOffset2D{0, 0}, extents};
			renderingInfo.layerCount = 1;
			renderingInfo.viewMask = 0;
			renderingInfo.colorAttachmentCount = 1;
			renderingInfo.pColorAttachments = colorAttachement;
			renderingInfo.pDepthAttachment = depthAttachement;
			renderingInfo.pStencilAttachment = nullptr;

			return renderingInfo;
		}

		inline static VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader, const char *entrypointName = "main") {
			MGN_CORE_ASSERT(entrypointName != nullptr, "[Vulkan] Cannot create a shader without a name.");

			VkPipelineShaderStageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			info.pNext = nullptr;
			info.stage = stage;
			info.module = shader;
			info.pName = entrypointName;
			return info;
		}


		inline static VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo() {
			VkPipelineLayoutCreateInfo info{};

			info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			info.pNext = nullptr;

			return info;
		}

		std::optional<std::shared_ptr<MeshAsset>> LoadCPUMesh(VulkanRenderer *engine, const Core::CPUMesh& mesh);
		std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadMeshes(VulkanRenderer *engine, const std::filesystem::path &filePath);
		void LoadModelAsDynamic(VulkanRenderer *renderer, Core::Scene *coreScene, Core::EntityID parent, const std::filesystem::path &filePath);

		std::shared_ptr<MeshAsset> LoadLines(VulkanRenderer* renderer, std::span<Core::LineObject> lines);
		ManualDeleteMeshAsset LoadManualLines(VulkanRenderer *renderer, std::span<Core::LineObject> lines);
		std::shared_ptr<MeshAsset> LoadPoints(VulkanRenderer* renderer, std::span<Core::Vertex> points);

	} // namespace Initializer
} // namespace Imagine::Vulkan
