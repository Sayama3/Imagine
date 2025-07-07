//
// Created by Iannis on 26/05/2025.
//

#pragma once


#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Vulkan/VulkanInitializer.hpp"

namespace Imagine::Vulkan {
	namespace Utils {
		inline static VkShaderStageFlagBits GetShaderStageFlagsBits(const Core::ShaderStage stage) {
			static_assert(sizeof(VkShaderStageFlagBits) == sizeof(int));
			int stages = 0;

			if (CHECK_SHADER_STAGE_BIT(stage, Imagine::Core::ShaderStage::Vertex)) {
				stages |= (int) VK_SHADER_STAGE_VERTEX_BIT;
			}
			if (CHECK_SHADER_STAGE_BIT(stage, Imagine::Core::ShaderStage::TessellationControl)) {
				stages |= (int) VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			}
			if (CHECK_SHADER_STAGE_BIT(stage, Imagine::Core::ShaderStage::TessellationEvaluation)) {
				stages |= (int) VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			}
			if (CHECK_SHADER_STAGE_BIT(stage, Imagine::Core::ShaderStage::Geometry)) {
				stages |= (int) VK_SHADER_STAGE_GEOMETRY_BIT;
			}
			if (CHECK_SHADER_STAGE_BIT(stage, Imagine::Core::ShaderStage::Fragment)) {
				stages |= (int) VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			if (CHECK_SHADER_STAGE_BIT(stage, Imagine::Core::ShaderStage::Compute)) {
				stages |= (int) VK_SHADER_STAGE_COMPUTE_BIT;
			}

			MGN_CORE_MASSERT(stages != 0, "The shader stage '{}' in unknown.", (uint16_t) stage);

			return (VkShaderStageFlagBits) stages;
		}
		// TODO: Remake the function to be a bit more accurate with what needs to be done in the engine.
		inline static void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT) {
			VkImageMemoryBarrier2 imageBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
			imageBarrier.pNext = nullptr;

			// TODO: Refine the way we assign and find the masks.
			imageBarrier.srcStageMask = srcStageMask;
			imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
			imageBarrier.dstStageMask = dstStageMask;
			imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

			imageBarrier.oldLayout = currentLayout;
			imageBarrier.newLayout = newLayout;

			const VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier.subresourceRange = Initializer::ImageSubresourceRange(aspectMask);
			imageBarrier.image = image;

			VkDependencyInfo depInfo{};
			depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			depInfo.pNext = nullptr;

			depInfo.imageMemoryBarrierCount = 1;
			depInfo.pImageMemoryBarriers = &imageBarrier;

			vkCmdPipelineBarrier2(cmd, &depInfo);
		}

		// TODO: Make my own version using compute shaders. Only used to setup the engine with something working.
		inline static void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize, VkImageAspectFlags aspectMask) {
			VkImageBlit2 blitRegion{.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr};

			// TODO: Check if we shouldn't use the index 0.
			blitRegion.srcOffsets[1].x = srcSize.width;
			blitRegion.srcOffsets[1].y = srcSize.height;
			blitRegion.srcOffsets[1].z = 1;

			blitRegion.dstOffsets[1].x = dstSize.width;
			blitRegion.dstOffsets[1].y = dstSize.height;
			blitRegion.dstOffsets[1].z = 1;

			// TODO: Take into account the mip maps if created.
			blitRegion.srcSubresource.aspectMask = aspectMask;
			blitRegion.srcSubresource.baseArrayLayer = 0;
			blitRegion.srcSubresource.layerCount = 1;
			blitRegion.srcSubresource.mipLevel = 0;

			// TODO: Take into account the mip maps if created.
			blitRegion.dstSubresource.aspectMask = aspectMask;
			blitRegion.dstSubresource.baseArrayLayer = 0;
			blitRegion.dstSubresource.layerCount = 1;
			blitRegion.dstSubresource.mipLevel = 0;

			VkBlitImageInfo2 blitInfo{.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr};
			blitInfo.dstImage = destination;
			blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			blitInfo.srcImage = source;
			blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			blitInfo.filter = VK_FILTER_LINEAR;
			blitInfo.regionCount = 1;
			blitInfo.pRegions = &blitRegion;

			vkCmdBlitImage2(cmd, &blitInfo);
		}
		inline static bool LoadShaderModule(const Core::Buffer &shaderBuffer, VkDevice device, VkShaderModule *outShaderModule) {

			// create a new shader module, using the buffer we loaded
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.pNext = nullptr;

			// codeSize has to be in bytes
			createInfo.codeSize = shaderBuffer.Size();
			createInfo.pCode = shaderBuffer.Get<uint32_t>();

			// check that the creation goes well.
			VkShaderModule shaderModule;
			VkResult err = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
			if (err != VK_SUCCESS) {
				MGN_CORE_ERROR("[Vulkan] Shader Loading Failed : {}", string_VkResult(err));
				return false;
			}
			*outShaderModule = shaderModule;
			return true;
		}

		inline static bool LoadShaderModule(const char *filePath, VkDevice device, VkShaderModule *outShaderModule) {
			const Core::Buffer buffer = Core::FileSystem::readBinaryFile(filePath);
			return LoadShaderModule(buffer, device, outShaderModule);
		}

		inline static bool LoadShaderModule(const std::filesystem::path &filePath, VkDevice device, VkShaderModule *outShaderModule) {
			const Core::Buffer buffer = Core::FileSystem::readBinaryFile(filePath);
			return LoadShaderModule(buffer, device, outShaderModule);
		}

		inline static VkFormat GetImageVkFormat(const Core::ImagePixelType pxType, const uint32_t channels) {
			VkFormat fmt = VK_FORMAT_UNDEFINED;
			switch (pxType) {
				case Core::ImagePixelType::Uint8:
					switch (channels) {
						case 1:
							fmt = VK_FORMAT_R8_UNORM;
							break;
						case 2:
							fmt = VK_FORMAT_R8G8_UNORM;
							break;
						case 3:
							fmt = VK_FORMAT_R8G8B8_UNORM;
							break;
						case 4:
							fmt = VK_FORMAT_R8G8B8A8_UNORM;
							break;
						default:
							break;
					}
					break;
				case Core::ImagePixelType::Uint16:
					switch (channels) {
						case 1:
							fmt = VK_FORMAT_R16_UNORM;
							break;
						case 2:
							fmt = VK_FORMAT_R16G16_UNORM;
							break;
						case 3:
							fmt = VK_FORMAT_R16G16B16_UNORM;
							break;
						case 4:
							fmt = VK_FORMAT_R16G16B16A16_UNORM;
							break;
						default:
							break;
					}
					break;
				case Core::ImagePixelType::Uint32:
					switch (channels) {
						case 1:
							fmt = VK_FORMAT_R32_UINT;
							break;
						case 2:
							fmt = VK_FORMAT_R32G32_UINT;
							break;
						case 3:
							fmt = VK_FORMAT_R32G32B32_UINT;
							break;
						case 4:
							fmt = VK_FORMAT_R32G32B32A32_UINT;
							break;
						default:
							break;
					}
					break;
				case Core::ImagePixelType::Int8:
					switch (channels) {
						case 1:
							fmt = VK_FORMAT_R8_SNORM;
							break;
						case 2:
							fmt = VK_FORMAT_R8G8_SNORM;
							break;
						case 3:
							fmt = VK_FORMAT_R8G8B8_SNORM;
							break;
						case 4:
							fmt = VK_FORMAT_R8G8B8A8_SNORM;
							break;
						default:
							break;
					}
					break;
				case Core::ImagePixelType::Int16:
					switch (channels) {
						case 1:
							fmt = VK_FORMAT_R16_SNORM;
							break;
						case 2:
							fmt = VK_FORMAT_R16G16_SNORM;
							break;
						case 3:
							fmt = VK_FORMAT_R16G16B16_SNORM;
							break;
						case 4:
							fmt = VK_FORMAT_R16G16B16A16_SNORM;
							break;
						default:
							break;
					}
					break;
				case Core::ImagePixelType::Int32:
					switch (channels) {
						case 1:
							fmt = VK_FORMAT_R32_SINT;
							break;
						case 2:
							fmt = VK_FORMAT_R32G32_SINT;
							break;
						case 3:
							fmt = VK_FORMAT_R32G32B32_SINT;
							break;
						case 4:
							fmt = VK_FORMAT_R32G32B32A32_SINT;
							break;
						default:
							break;
					}
					break;
				case Core::ImagePixelType::F32:
					switch (channels) {
						case 1:
							fmt = VK_FORMAT_R32_SFLOAT;
							break;
						case 2:
							fmt = VK_FORMAT_R32G32_SFLOAT;
							break;
						case 3:
							fmt = VK_FORMAT_R32G32B32_SFLOAT;
							break;
						case 4:
							fmt = VK_FORMAT_R32G32B32A32_SFLOAT;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
			MGN_CORE_MASSERT(fmt != VK_FORMAT_UNDEFINED, "The pixel format {} with {} channels is not implemented.", pxType, channels);
			return fmt;
		}

		template<typename PixelType>
		inline static VkFormat GetImageVkFormat(const Core::Image<PixelType> &image) {
			return GetImageVkFormat(Core::Image<PixelType>::GetPixelType(), image.channels);
		}
	} // namespace Utils
} // namespace Imagine::Vulkan
