//
// Created by Iannis on 26/05/2025.
//

#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace Imagine::Vulkan
{
    namespace Initializer
    {
        inline static VkCommandPoolCreateInfo CommandPoolCreateInfo(const uint32_t queueFamilyIndex, const VkCommandPoolCreateFlags flags = 0)
        {
            VkCommandPoolCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.pNext = nullptr;

            info.queueFamilyIndex = queueFamilyIndex;
            info.flags = flags;
            return info;
        }

        inline static VkCommandBufferAllocateInfo  CommandBufferAllocateInfo(VkCommandPool pool, const uint32_t count = 1)
        {
            VkCommandBufferAllocateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.pNext = nullptr;

            info.commandPool = pool;
            info.commandBufferCount = count;
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //TODO: See what the thing is all about with the secondary buffers
            return info;
        }

        inline static VkFenceCreateInfo FenceCreateInfo(const VkFenceCreateFlags flags = 0)
        {
            VkFenceCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = flags;

            return info;
        }

        inline static VkSemaphoreCreateInfo SemaphoreCreateInfo(const VkSemaphoreCreateFlags flags = 0)
        {
            VkSemaphoreCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = flags;
            return info;
        }

        inline static VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferUsageFlags flags = 0)
        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.pNext = nullptr;

            info.pInheritanceInfo = nullptr;
            info.flags = flags;
            return info;
        }

        inline static VkImageSubresourceRange ImageSubresourceRange(const VkImageAspectFlags aspectMask)
        {
            VkImageSubresourceRange subImage = {};
            subImage.aspectMask = aspectMask;
            subImage.baseMipLevel = 0;
            subImage.levelCount = VK_REMAINING_MIP_LEVELS;
            subImage.baseArrayLayer = 0;
            subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

            return subImage;
        }

        VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
        {
            VkSemaphoreSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
            submitInfo.pNext = nullptr;
            submitInfo.semaphore = semaphore;
            submitInfo.stageMask = stageMask;
            submitInfo.deviceIndex = 0;
            submitInfo.value = 1;

            return submitInfo;
        }

        VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd)
        {
            VkCommandBufferSubmitInfo info{};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
            info.pNext = nullptr;
            info.commandBuffer = cmd;
            info.deviceMask = 0;

            return info;
        }

        VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo)
        {
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

    }
}
