//
// Created by Iannis on 26/05/2025.
//

#pragma once

#include "Imagine/Vulkan/VulkanInitializer.hpp"

namespace Imagine::Vulkan
{
    namespace Utils
    {
        //TODO: Remake the function to be a bit more accurate with what needs to be done in the engine.
        inline static void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout, VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT)
        {
            VkImageMemoryBarrier2 imageBarrier {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
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

            VkDependencyInfo depInfo {};
            depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            depInfo.pNext = nullptr;

            depInfo.imageMemoryBarrierCount = 1;
            depInfo.pImageMemoryBarriers = &imageBarrier;

            vkCmdPipelineBarrier2(cmd, &depInfo);
        }

    	//TODO: Make my own version using compute shaders. Only used to setup the engine with something working.
    	inline static void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize, VkImageAspectFlags aspectMask) {
        	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

        	//TODO: Check if we shouldn't use the index 0.
        	blitRegion.srcOffsets[1].x = srcSize.width;
        	blitRegion.srcOffsets[1].y = srcSize.height;
        	blitRegion.srcOffsets[1].z = 1;

        	blitRegion.dstOffsets[1].x = dstSize.width;
        	blitRegion.dstOffsets[1].y = dstSize.height;
        	blitRegion.dstOffsets[1].z = 1;

        	//TODO: Take into account the mip maps if created.
        	blitRegion.srcSubresource.aspectMask = aspectMask;
        	blitRegion.srcSubresource.baseArrayLayer = 0;
        	blitRegion.srcSubresource.layerCount = 1;
        	blitRegion.srcSubresource.mipLevel = 0;

        	//TODO: Take into account the mip maps if created.
        	blitRegion.dstSubresource.aspectMask = aspectMask;
        	blitRegion.dstSubresource.baseArrayLayer = 0;
        	blitRegion.dstSubresource.layerCount = 1;
        	blitRegion.dstSubresource.mipLevel = 0;

        	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
        	blitInfo.dstImage = destination;
        	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        	blitInfo.srcImage = source;
        	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        	blitInfo.filter = VK_FILTER_LINEAR;
        	blitInfo.regionCount = 1;
        	blitInfo.pRegions = &blitRegion;

        	vkCmdBlitImage2(cmd, &blitInfo);
        }
    }
}