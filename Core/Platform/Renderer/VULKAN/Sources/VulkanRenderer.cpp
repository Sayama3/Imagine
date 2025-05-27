//
// Created by Sayama on 23/05/2025.
//

// --- other includes ---


//bootstrap library
#include <VkBootstrap.h>

#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Application/Application.hpp"
#include "Imagine/Vulkan/VulkanInitializer.hpp"
#include "Imagine/Vulkan/VulkanUtils.hpp"
#include "Imagine/Vulkan/VulkanMacros.hpp"

#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Application/Window.hpp"

// Window-Specific functions used to create the Surface until I figure out a way to properly create it from the Window Side.
#if defined(MGN_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#elif defined(MGN_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#else
#error "Vulkan Rendering without window is not implemented yet."
#endif

using namespace Imagine::Core;

namespace Imagine::Vulkan
{
    VulkanRenderer::VulkanRenderer(const ApplicationParameters& appParams) : Renderer(), m_RenderParams(appParams.Renderer.value()), m_AppParams(appParams)
    {
        InitializeVulkan();
        InitializeSwapChain();
        InitializeCommands();
        InitializeSyncStructures();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        ShutdownVulkan();
    }

    void VulkanRenderer::InitializeVulkan()
    {
        vkb::InstanceBuilder builder;

        //make the vulkan instance, with basic debug features
        auto inst_ret = builder.set_app_name(m_AppParams.AppName.c_str())
                               .set_engine_name(ApplicationParameters::EngineName)
                               .request_validation_layers(m_RenderParams.EnableDebug)
                               .use_default_debug_messenger()
                               .require_api_version(1, 3, 0) // TODO? See if I better use the 1.2.0 vulkan version.
                               .build();

        if (!inst_ret.has_value())
        {
            throw std::runtime_error("Vulkan 1.3.0 couldn't be initialized.");
        }

        vkb::Instance vkb_inst = inst_ret.value();

        m_Instance = inst_ret->instance;
        m_DebugMessenger = inst_ret->debug_messenger;

        //TODO: See how it's done in Metal/DX12/... to see the best way to implement the surface creation.
#if defined(MGN_WINDOW_SDL3)
        SDL_Vulkan_CreateSurface(reinterpret_cast<struct SDL_Window*>(Core::Window::Get()->GetWindowPtr()), m_Instance,
                                 nullptr, &m_Surface);
#elif defined(MGN_WINDOW_GLFW)
		glfwCreateWindowSurface(m_Instance, reinterpret_cast<struct GLFWwindow*>(Core::Window::Get()->GetWindowPtr()), nullptr, &m_Surface);
#endif

        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        features.dynamicRendering = true;
        features.synchronization2 = true;

        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;


        //use vkbootstrap to select a gpu.
        //We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
        vkb::PhysicalDeviceSelector selector{vkb_inst};
        vkb::PhysicalDevice physicalDevice = selector
                                             .set_minimum_version(1, 3)
                                             .set_required_features_13(features)
                                             .set_required_features_12(features12)
                                             .set_surface(m_Surface)
                                             .select()
                                             .value();


        //create the final vulkan device
        vkb::DeviceBuilder deviceBuilder{physicalDevice};

        vkb::Device vkbDevice = deviceBuilder.build().value();

        // Get the VkDevice handle used in the rest of a vulkan application
        m_Device = vkbDevice.device;
        m_ChosenGPU = physicalDevice.physical_device;

        m_Frames.resize(m_RenderParams.NbrFrameInFlight);

        m_GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

        // initialize the memory allocator
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_ChosenGPU;
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocatorInfo, &m_Allocator);

        m_MainDeletionQueue.push_back(m_Allocator);
    }

    void VulkanRenderer::InitializeSwapChain()
    {
        const Size2 framebufferSize = Window::Get()->GetFramebufferSize();
        CreateSwapChain(framebufferSize.width, framebufferSize.height);

        //draw image size will match the window
        VkExtent3D drawImageExtent = {
            framebufferSize.width,
            framebufferSize.height,
            1
        };

        //hardcoding the draw format to 32 bit float
        m_DrawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        m_DrawImage.imageExtent = drawImageExtent;

        VkImageUsageFlags drawImageUsages{};
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo rimg_info = Initializer::ImageCreateInfo2D(m_DrawImage.imageFormat, drawImageUsages, drawImageExtent);

        //for the draw image, we want to allocate it from gpu local memory
        VmaAllocationCreateInfo rimg_allocinfo = {};
        rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        //allocate and create the image
        vmaCreateImage(m_Allocator, &rimg_info, &rimg_allocinfo, &m_DrawImage.image, &m_DrawImage.allocation, nullptr);

        //build a image-view for the draw image to use for rendering
        VkImageViewCreateInfo rview_info = Initializer::ImageViewCreateInfo2D(m_DrawImage.imageFormat, m_DrawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_CHECK(vkCreateImageView(m_Device, &rview_info, nullptr, &m_DrawImage.imageView));

        //add to deletion queues
        m_MainDeletionQueue.push_back(Deleter::VmaImage{m_Allocator, m_DrawImage.allocation, m_DrawImage.image});
        m_MainDeletionQueue.push_back(m_DrawImage.imageView);
    }

    void VulkanRenderer::InitializeCommands()
    {
        //create a command pool for commands submitted to the graphics queue.
        //we also want the pool to allow for resetting of individual command buffers
        VkCommandPoolCreateInfo commandPoolInfo = Initializer::CommandPoolCreateInfo(m_GraphicsQueueFamily,VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (int i = 0; i < m_RenderParams.NbrFrameInFlight; i++)
        {
            VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_Frames[i].m_CommandPool));

            // allocate the default command buffer that we will use for rendering
            VkCommandBufferAllocateInfo cmdAllocInfo = Initializer::CommandBufferAllocateInfo(m_Frames[i].m_CommandPool, 1);

            VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_Frames[i].m_MainCommandBuffer));
        }
    }

    void VulkanRenderer::InitializeSyncStructures()
    {
        //create synchronization structures
        //one fence to control when the gpu has finished rendering the frame,
        //and 2 semaphores to synchronize rendering with swapchain
        //we want the fence to start signalled so we can wait on it on the first frame
        const VkFenceCreateInfo fenceCreateInfo = Initializer::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        const VkSemaphoreCreateInfo semaphoreCreateInfo = Initializer::SemaphoreCreateInfo();

        for (auto& frame : m_Frames)
        {
            VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &frame.m_RenderFence));
            VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &frame.m_SwapchainSemaphore));
            VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &frame.m_RenderSemaphore));
        }
    }

    void VulkanRenderer::CreateSwapChain(const uint32_t width, const uint32_t height)
    {
        vkb::SwapchainBuilder swapchainBuilder{m_ChosenGPU, m_Device, m_Surface};

        m_SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM; //TODO: See with HDR if we really want this one.

        vkb::Swapchain vkbSwapchain = swapchainBuilder
                                      //.use_default_format_selection()
                                      .set_desired_format(VkSurfaceFormatKHR{
                                          .format = m_SwapchainImageFormat,
                                          .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
                                      })
                                      //use vsync present mode
                                      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                      // Hard VSync. Will limit the refreshrate to the one of the monitor.
                                      .set_desired_extent(width, height)
                                      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                                      .build()
                                      .value();

        m_SwapchainExtent = vkbSwapchain.extent;

        //store swapchain and its related images
        m_Swapchain = vkbSwapchain.swapchain;
        m_SwapchainImages = vkbSwapchain.get_images().value();
        m_SwapchainImageViews = vkbSwapchain.get_image_views().value();
    }

    void VulkanRenderer::DestroySwapChain()
    {
        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        m_Swapchain = nullptr;

        // destroy swapchain resources
        for (auto& m_SwapchainImageView : m_SwapchainImageViews)
        {
            vkDestroyImageView(m_Device, m_SwapchainImageView, nullptr);
        }
        m_SwapchainImages.clear();
        m_SwapchainImageViews.clear();
    }

    void VulkanRenderer::ShutdownVulkan()
    {
        vkDeviceWaitIdle(m_Device);

        for (auto& frame : m_Frames) {
            vkDestroyCommandPool(m_Device, frame.m_CommandPool, nullptr);

            //destroy sync objects
            vkDestroyFence(m_Device,  frame.m_RenderFence, nullptr);
            vkDestroySemaphore(m_Device,  frame.m_RenderSemaphore, nullptr);
            vkDestroySemaphore(m_Device , frame.m_SwapchainSemaphore, nullptr);

            frame.m_DeletionQueue.flush();
        }
        m_Frames.clear();

        m_MainDeletionQueue.flush();

        DestroySwapChain();

        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyDevice(m_Device, nullptr);

        vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        vkDestroyInstance(m_Instance, nullptr);
    }

    VulkanFrameData& VulkanRenderer::GetCurrentFrame()
    {
        return m_Frames[m_FrameIndex];
    }

    void VulkanRenderer::Draw() {

		// wait until the gpu has finished rendering the last frame. Timeout of 1
		// second
		VK_CHECK(vkWaitForFences(m_Device, 1, &GetCurrentFrame().m_RenderFence, true, 1000000000));
		VK_CHECK(vkResetFences(m_Device, 1, &GetCurrentFrame().m_RenderFence));

		GetCurrentFrame().m_DeletionQueue.flush();

		// request image from the swapchain
		uint32_t swapchainImageIndex;
		VK_CHECK(vkAcquireNextImageKHR(m_Device, m_Swapchain, 1000000000, GetCurrentFrame().m_SwapchainSemaphore, nullptr, &swapchainImageIndex));

    	VkCommandBuffer cmd{nullptr};
		cmd = GetCurrentFrame().m_MainCommandBuffer;
		VK_CHECK(vkResetCommandBuffer(cmd, 0));

		const auto beginInfo = Initializer::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

		// make the swapchain image into writeable mode before rendering
		// TODO: See if the layout VK_IMAGE_LAYOUT_GENERAL is the most optimal

    	// Prepare the image for the drawing
		Utils::TransitionImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    	// ALl the draw command starting with the clearing of the background
    	DrawBackground(cmd);

    	// Ending the drawing commands and copying the data into the swapchain image.
		Utils::TransitionImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		Utils::TransitionImage(cmd, m_SwapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    	Utils::CopyImageToImage(cmd, m_DrawImage.image, m_SwapchainImages[swapchainImageIndex], m_DrawExtent, m_SwapchainExtent, VK_IMAGE_ASPECT_COLOR_BIT);

    	// make the swapchain image into presentable mode
    	Utils::TransitionImage(cmd, m_SwapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		// finalize the command buffer (we can no longer add commands, but it can now be executed)
		VK_CHECK(vkEndCommandBuffer(cmd));

		// prepare the submission to the queue.
		// we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
		// we will signal the _renderSemaphore, to signal that rendering has finished

		VkCommandBufferSubmitInfo cmdinfo = Initializer::CommandBufferSubmitInfo(cmd);

		VkSemaphoreSubmitInfo waitInfo = Initializer::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().m_SwapchainSemaphore);
		VkSemaphoreSubmitInfo signalInfo = Initializer::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().m_RenderSemaphore);

		VkSubmitInfo2 submit = Initializer::SubmitInfo(&cmdinfo, &signalInfo, &waitInfo);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit2(m_GraphicsQueue, 1, &submit, GetCurrentFrame().m_RenderFence));

		// prepare present
		//  this will put the image we just rendered to into the visible window.
		//  we want to wait on the _renderSemaphore for that,
		//  as its necessary that drawing commands have finished before the image is displayed to the user
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &GetCurrentFrame().m_RenderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchainImageIndex;

		VK_CHECK(vkQueuePresentKHR(m_GraphicsQueue, &presentInfo));

		m_FrameIndex = (m_FrameIndex + 1) % m_RenderParams.NbrFrameInFlight;
	}

	void VulkanRenderer::DrawBackground(VkCommandBuffer cmd) {
    	// make a clear-color from Time.
    	VkClearColorValue clearValue;
    	float flash = std::abs(std::sin(static_cast<float>(Application::Get()->Time())));
    	clearValue = {{0.0f, 0.0f, flash, 1.0f}};

    	VkImageSubresourceRange clearRange = Initializer::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    	// clear image
    	vkCmdClearColorImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
	}
} // namespace Imagine::Vulkan
