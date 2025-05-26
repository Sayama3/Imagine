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
    }
}