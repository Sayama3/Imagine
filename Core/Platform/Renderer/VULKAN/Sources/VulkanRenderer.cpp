//
// Created by Sayama on 23/05/2025.
//

// --- other includes ---
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

//bootstrap library
#include <VkBootstrap.h>

#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Application/Window.hpp"

#if defined(MGN_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#elif defined(MGN_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#else
#error "Vulkan Rendering without window is not implemented yet."
#endif

#define VK_CHECK(vk_func) \
{\
    VkResult err = vk_func; \
    if(err) { \
        const std::string vulkan_error_str = std::string{"[Vulkan] Detected error: "} + std::string{string_VkResult(err)};\
        MGN_CORE_ERROR(vulkan_error_str); \
        throw std::runtime_error(vulkan_error_str);\
    }\
}\

using namespace Imagine::Core;

namespace Imagine::Vulkan
{
    VulkanRenderer::VulkanRenderer(const RendererParameters& renderParams, const ApplicationParameters& appParams) :
        Renderer(), m_RenderParams(renderParams), m_AppParams(appParams)
    {
        InitializeVulkan();
        InitializeSwapchain();
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
#elif
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
    }

    void VulkanRenderer::InitializeSwapchain()
    {
        CreateSwapChain(Window::Get()->GetFramebufferWidth(), Window::Get()->GetFramebufferHeight());
    }

    void VulkanRenderer::InitializeCommands()
    {
        //create a command pool for commands submitted to the graphics queue.
        //we also want the pool to allow for resetting of individual command buffers
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.pNext = nullptr;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamily;

        for (int i = 0; i < m_RenderParams.NbrFrameInFlight; i++)
        {
            VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_Frames[i].m_CommandPool));

            // allocate the default command buffer that we will use for rendering
            VkCommandBufferAllocateInfo cmdAllocInfo = {};
            cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdAllocInfo.pNext = nullptr;
            cmdAllocInfo.commandPool = m_Frames[i].m_CommandPool;
            cmdAllocInfo.commandBufferCount = 1;
            cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_Frames[i].m_MainCommandBuffer));
        }
    }

    void VulkanRenderer::InitializeSyncStructures()
    {
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
        for (int i = 0; i < m_SwapchainImageViews.size(); i++)
        {
            vkDestroyImageView(m_Device, m_SwapchainImageViews[i], nullptr);
        }
        m_SwapchainImages.clear();
        m_SwapchainImageViews.clear();
    }

    void VulkanRenderer::ShutdownVulkan()
    {
        DestroySwapChain();

        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyDevice(m_Device, nullptr);

        vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        vkDestroyInstance(m_Instance, nullptr);
    }
}
