//
// Created by Sayama on 23/05/2025.
//

// --- other includes ---


// bootstrap library
#define VMA_IMPLEMENTATION 1

#include <VkBootstrap.h>

#include "Imagine/Application/Application.hpp"
#include "Imagine/Vulkan/VulkanInitializer.hpp"
#include "Imagine/Vulkan/VulkanMacros.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

#include <vk_mem_alloc.h>

#include "Imagine/Vulkan/VulkanUtils.hpp"

#include "Imagine/Application/Window.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Scene/Renderable.hpp"
#include "Imagine/Scene/SceneManager.hpp"
#include "Imagine/Vulkan/Descriptors.hpp"
#include "Imagine/Vulkan/PipelineBuilder.hpp"

// Window-Specific functions used to create the Surface until I figure out a way to properly create it from the Window Side.
#if defined(MGN_WINDOW_SDL3)
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#elif defined(MGN_WINDOW_GLFW)
#include <GLFW/glfw3.h>
#else
#error "Vulkan Rendering without window is not implemented yet."
#endif

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#endif

using namespace Imagine::Core;
using namespace Imagine::Literal;

namespace Imagine::Vulkan {
	constexpr bool c_BreakOnError = false;

	static inline VkBool32 VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
		const char *mt = vkb::to_string_message_type(messageType);

		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
			switch (messageSeverity) {
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
					MGN_CORE_TRACE("[Vulkan: {}] [{}] - {}", mt, pCallbackData->pMessageIdName, pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
					MGN_CORE_INFO("[Vulkan: {}] [{}] - {}", mt, pCallbackData->pMessageIdName, pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
					MGN_CORE_WARNING("[Vulkan: {}] [{}] - {}", mt, pCallbackData->pMessageIdName, pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
					MGN_CORE_ERROR("[Vulkan: {}] [{}] - {}", mt, pCallbackData->pMessageIdName, pCallbackData->pMessage);
					if constexpr (c_BreakOnError) {
						MGN_BREAK();
					}
					break;
				default:
					MGN_CORE_ERROR("[Vulkan: {}] [{}] - {}", mt, pCallbackData->pMessageIdName, pCallbackData->pMessage);
					break;
			}
		}
		else {
			switch (messageSeverity) {
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
					MGN_CORE_TRACE("[Vulkan: {}] - {}", mt, pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
					MGN_CORE_INFO("[Vulkan: {}] - {}", mt, pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
					MGN_CORE_WARNING("[Vulkan: {}] - {}", mt, pCallbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
					MGN_CORE_ERROR("[Vulkan: {}] - {}", mt, pCallbackData->pMessage);
					if constexpr (c_BreakOnError) {
						MGN_BREAK();
					}
					break;
				default:
					MGN_CORE_ERROR("[Vulkan: {}] - {}", mt, pCallbackData->pMessage);
					break;
			}
		}

		return VK_FALSE; // Applications must return false here (Except Validation, if return true, will skip calling to driver)
	}

	VulkanRenderer::VulkanRenderer(const ApplicationParameters &appParams) :
		Renderer(), m_AppParams(appParams) {
		InitializeVulkan();
		InitializeSwapChain();
		m_DrawExtent.width = std::ceil(std::min(m_SwapchainExtent.width, m_DrawImage.imageExtent.width) * m_RenderScale);
		m_DrawExtent.height = std::ceil(std::min(m_SwapchainExtent.height, m_DrawImage.imageExtent.height) * m_RenderScale);
		InitializeCommands();
		InitializeSyncStructures();
		CreateDefaultSamplers();
		InitializeDescriptors();
		InitializePipelines();

		InitDefaultData();

		// Initialize the cache of matrix a first time.
		UpdateCache();
	}

	VulkanRenderer::~VulkanRenderer() {
		PrepareShutdown();
		ShutdownVulkan();
	}

	VulkanRenderer *VulkanRenderer::Get() {
		Renderer *renderer = Renderer::Get();
		if (!renderer) return nullptr;
		if (renderer->GetAPI() != VulkanRenderer::GetStaticAPI()) return nullptr;
		return reinterpret_cast<VulkanRenderer *>(renderer);
	}

	void VulkanRenderer::PrepareShutdown() {
		vkDeviceWaitIdle(m_Device);
	}

	Mat4 VulkanRenderer::GetViewMatrix() const {
		return Camera::s_MainCamera->GetViewMatrix();
	}

	Mat4 VulkanRenderer::GetProjectionMatrix() const {
		Mat4 proj = glm::perspective(glm::radians(Real(70)), (Real) m_DrawExtent.width / (Real) m_DrawExtent.height, Real(0.1), Real(10000.));
		// Inverse Y to have up toward up
		proj[1][1] *= -1;
		return proj;
	}

	Vec3 VulkanRenderer::GetWorldPoint(const Vec2 screenPoint) const {
		const auto viewport = GetViewport();
		const auto viewportSize = viewport.GetSize();
		const Vec2 normalizeMousePos = Vec2{(screenPoint.x / viewportSize.x) * 2_r - 1_r, (screenPoint.y / viewportSize.y) * 2_r - 1_r};
		const Vec4 result = InvViewProjectMatrixCached * Vec4{normalizeMousePos.x, normalizeMousePos.y, 0, 1};
		const Vec4 resultNormalize = result / result.w;
		return resultNormalize;
	}

	Mat4 VulkanRenderer::GetViewProjectMatrix() const {
		return GetProjectionMatrix() * GetViewMatrix();
	}

	Rect<> VulkanRenderer::GetViewport() const {
#ifdef MGN_IMGUI
		if (MgnImGui::DockingEnabled() && m_ImGuiViewport.has_value()) {
			return m_ImGuiViewport.value();
		}
		else {
			return Window::Get()->GetWindowRect();
		}
#else
		return Window::Get()->GetWindowRect();
#endif
	}
	VkDevice VulkanRenderer::GetDevice() {
		return m_Device;
	}

	VkPhysicalDevice VulkanRenderer::GetPhysicalDevice() {
		return m_PhysicalDevice;
	}
	VmaAllocator VulkanRenderer::GetAllocator() {
		return m_Allocator;
	}

	void VulkanRenderer::InitializeVulkan() {
		vkb::InstanceBuilder builder;

		// make the vulkan instance, with basic debug features
		builder.set_app_name(m_AppParams.AppName.c_str())
				.set_engine_name(ApplicationParameters::EngineName)
				.require_api_version(1, 3, 0); // TODO? See if I better use the 1.2.0 vulkan version.

		if (GetRenderParams().EnableDebug) {
			builder.request_validation_layers(GetRenderParams().EnableDebug)
					.set_debug_callback(VkDebugCallback);
		}
		auto inst_ret = builder.build();
		if (!inst_ret.has_value()) {
			throw std::runtime_error("Vulkan 1.3.0 couldn't be initialized.");
		}

		vkb::Instance vkb_inst = inst_ret.value();

		m_Instance = inst_ret->instance;
		m_DebugMessenger = inst_ret->debug_messenger;

		// TODO: See how it's done in Metal/DX12/... to see the best way to implement the surface creation.
#if defined(MGN_WINDOW_SDL3)
		SDL_Vulkan_CreateSurface(reinterpret_cast<struct SDL_Window *>(Core::Window::Get()->GetWindowPtr()), m_Instance,
								 nullptr, &m_Surface);
#elif defined(MGN_WINDOW_GLFW)
		glfwCreateWindowSurface(m_Instance, reinterpret_cast<struct GLFWwindow *>(Core::Window::Get()->GetWindowPtr()), nullptr, &m_Surface);
#endif

		// vulkan 1.3 features
		VkPhysicalDeviceVulkan13Features features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
		features.dynamicRendering = true;
		features.synchronization2 = true;

		// vulkan 1.2 features
		VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;

		// VkPhysicalDeviceVulkan11Features features11{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};


		// use vkbootstrap to select a gpu.
		// We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
		vkb::PhysicalDeviceSelector selector{vkb_inst};
		vkb::PhysicalDevice physicalDevice = selector
													 .set_minimum_version(1, 3)
													 .set_required_features_13(features)
													 .set_required_features_12(features12)
													 .set_surface(m_Surface)
													 .select()
													 .value();

		physicalDevice.enable_extension_if_present(VK_KHR_DISPLAY_EXTENSION_NAME);
		// physicalDevice.enable_features_if_present(VK_FEATURE)

		// create the final vulkan device
		vkb::DeviceBuilder deviceBuilder{physicalDevice};
		vkb::Device vkbDevice = deviceBuilder.build().value();

		// Get the VkDevice handle used in the rest of a vulkan application
		m_Device = vkbDevice.device;
		m_PhysicalDevice = physicalDevice.physical_device;

		m_Frames.resize(GetRenderParams().NbrFrameInFlight);

		m_GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		// initialize the memory allocator
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.instance = m_Instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);

		m_MainDeletionQueue.push(m_Allocator);
	}

	void VulkanRenderer::InitializeSwapChain() {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

		VkExtent2D maxDrawImage{deviceProperties.limits.maxImageDimension2D, deviceProperties.limits.maxImageDimension2D};

		const Size2 framebufferSize = Window::Get()->GetFramebufferSize();
		CreateSwapChain(framebufferSize.width, framebufferSize.height);

		// draw image size will match the window
		// Max screen size currently usable is Samsung G3 {7680u, 2160u}
		VkExtent3D drawImageExtent = {
				std::min(maxDrawImage.width,std::max(7680u, framebufferSize.width)),
				std::min(maxDrawImage.height,std::max(2160u, framebufferSize.height)),
				1};

		// hardcoding the draw format to 16-bit float
		m_DrawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_DrawImage.imageExtent = drawImageExtent;

		VkImageUsageFlags drawImageUsages{};
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_SAMPLED_BIT;
		// drawImageUsages |= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		VkImageCreateInfo rimg_info = Initializer::ImageCreateInfo2D(m_DrawImage.imageFormat, drawImageUsages, drawImageExtent);

		// for the draw image, we want to allocate it from gpu local memory
		VmaAllocationCreateInfo rimg_allocinfo = {};
		rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// allocate and create the image
		vmaCreateImage(m_Allocator, &rimg_info, &rimg_allocinfo, &m_DrawImage.image, &m_DrawImage.allocation, nullptr);

		// build a image-view for the draw image to use for rendering
		VkImageViewCreateInfo rview_info = Initializer::ImageViewCreateInfo2D(m_DrawImage.imageFormat, m_DrawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

		VK_CHECK(vkCreateImageView(m_Device, &rview_info, nullptr, &m_DrawImage.imageView));

		// add to deletion queues
		m_MainDeletionQueue.push(Deleter::VmaImage{m_Allocator, m_DrawImage.allocation, m_DrawImage.image});
		m_MainDeletionQueue.push(m_DrawImage.imageView);


		m_DepthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
		m_DepthImage.imageExtent = drawImageExtent;
		VkImageUsageFlags depthImageUsages{};
		depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		VkImageCreateInfo dimg_info = Initializer::ImageCreateInfo2D(m_DepthImage.imageFormat, depthImageUsages, drawImageExtent);
		// allocate and create the image
		vmaCreateImage(m_Allocator, &dimg_info, &rimg_allocinfo, &m_DepthImage.image, &m_DepthImage.allocation, nullptr);
		// build a image-view for the draw image to use for rendering
		VkImageViewCreateInfo dview_info = Initializer::ImageViewCreateInfo2D(m_DepthImage.imageFormat, m_DepthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

		VK_CHECK(vkCreateImageView(m_Device, &dview_info, nullptr, &m_DepthImage.imageView));

		// add to deletion queues
		m_MainDeletionQueue.push(Deleter::VmaImage{m_Allocator, m_DepthImage.allocation, m_DepthImage.image});
		m_MainDeletionQueue.push(m_DepthImage.imageView);
	}

	void VulkanRenderer::InitializeCommands() {
		// create a command pool for commands submitted to the graphics queue.
		// we also want the pool to allow for resetting of individual command buffers
		VkCommandPoolCreateInfo commandPoolInfo = Initializer::CommandPoolCreateInfo(m_GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		for (int i = 0; i < GetRenderParams().NbrFrameInFlight; i++) {
			VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_Frames[i].m_CommandPool));
			// allocate the default command buffer that we will use for rendering
			VkCommandBufferAllocateInfo cmdAllocInfo = Initializer::CommandBufferAllocateInfo(m_Frames[i].m_CommandPool, 1);

			VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_Frames[i].m_MainCommandBuffer));
		}

		VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_ImmCommandPool));

		// allocate the command buffer for immediate submits
		VkCommandBufferAllocateInfo cmdAllocInfo = Initializer::CommandBufferAllocateInfo(m_ImmCommandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_ImmCommandBuffer));

		m_MainDeletionQueue.push(m_ImmCommandPool);
	}

	void VulkanRenderer::InitializeSyncStructures() {
		// create synchronization structures
		// one fence to control when the gpu has finished rendering the frame,
		// and 2 semaphores to synchronize rendering with swapchain
		// we want the fence to start signalled so we can wait on it on the first frame
		const VkFenceCreateInfo fenceCreateInfo = Initializer::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		const VkSemaphoreCreateInfo semaphoreCreateInfo = Initializer::SemaphoreCreateInfo();

		for (auto &frame: m_Frames) {
			VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &frame.m_RenderFence));
			VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &frame.m_SwapchainSemaphore));
			VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &frame.m_RenderSemaphore));
		}

		VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_ImmFence));
		m_MainDeletionQueue.push(m_ImmFence);
	}

	void VulkanRenderer::InitializeDescriptors() {

		for (int i = 0; i < m_Frames.size(); ++i) {
			// create a descriptor pool
			std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = {
					{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
					{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
					{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
					{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4},
			};

			m_Frames[i].m_FrameDescriptors = DescriptorAllocatorGrowable{};
			m_Frames[i].m_FrameDescriptors.Init(m_Device, 1000, frame_sizes);
			m_MainDeletionQueue.push(m_Frames[i].m_FrameDescriptors);
		}

		// create a descriptor pool that will hold 10 sets with 1 image each
		std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
				{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
		};
		m_GlobalDescriptorAllocator.Init(m_Device, 10, sizes);

		// make the descriptor set layout for our compute draw
		{
			DescriptorLayoutBuilder builder;
			builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			m_DrawImageDescriptorLayout = builder.Build(m_Device, VK_SHADER_STAGE_COMPUTE_BIT);
			m_MainDeletionQueue.push(m_DrawImageDescriptorLayout);
		}
		{
			// allocate a descriptor set for our draw image
			m_DrawImageDescriptors = m_GlobalDescriptorAllocator.Allocate(m_Device, m_DrawImageDescriptorLayout);
			DescriptorWriter writer;
			writer.WriteImage(0, m_DrawImage.imageView, m_DefaultSamplerLinear, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			writer.UpdateSet(m_Device, m_DrawImageDescriptors);
		}

		// make the descriptor set layout for our compute draw
		{
			DescriptorLayoutBuilder builder;
			builder.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_ImGuiImageDescriptorLayout = builder.Build(m_Device, VK_SHADER_STAGE_FRAGMENT_BIT);
			m_MainDeletionQueue.push(m_ImGuiImageDescriptorLayout);
		}
		{
			// allocate a descriptor set for our draw image
			m_ImGuiImageDescriptors = m_GlobalDescriptorAllocator.Allocate(m_Device, m_ImGuiImageDescriptorLayout);
			DescriptorWriter writer;
			// writer.WriteImage(0, m_DrawImage.imageView, m_DefaultSamplerLinear, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			writer.WriteImage(0, m_DrawImage.imageView, m_DefaultSamplerLinear, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			writer.UpdateSet(m_Device, m_ImGuiImageDescriptors);
		}

		{
			DescriptorLayoutBuilder builder;
			builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			m_GpuSceneDataDescriptorLayout = builder.Build(m_Device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
			m_MainDeletionQueue.push(m_GpuSceneDataDescriptorLayout);
		}

		// make sure both the descriptor allocator and the new layout get cleaned up properly
		m_MainDeletionQueue.push(m_GlobalDescriptorAllocator);

		{
			DescriptorLayoutBuilder layoutBuilder;
			layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_SingleImageDescriptorLayout = layoutBuilder.Build(m_Device, VK_SHADER_STAGE_FRAGMENT_BIT);
			m_MainDeletionQueue.push(m_SingleImageDescriptorLayout);
		}
	}

	void VulkanRenderer::InitializePipelines() {
		InitGradientPipeline();
		InitSkyPipeline();
		InitTrianglePipeline();
		InitMeshPipeline();

		m_MetalRoughMaterial.topology = Topology::Triangle;
		m_MetalRoughMaterial.BuildPipeline(this);
		m_MainDeletionQueue.push(m_MetalRoughMaterial);

		m_LineMetalRoughMaterial.topology = Topology::Line;
		m_LineMetalRoughMaterial.BuildPipeline(this);
		m_MainDeletionQueue.push(m_LineMetalRoughMaterial);

		// m_PointMetalRoughMaterial.topology = Topology::Point;
		// m_PointMetalRoughMaterial.BuildPipeline(this);
		// m_MainDeletionQueue.push(m_PointMetalRoughMaterial);
	}

	void VulkanRenderer::InitGradientPipeline() {
		ComputeEffect gradient;
		gradient.layout = nullptr;
		gradient.name = "gradient";
		gradient.data = {};

		// default colors
		gradient.data.data1 = glm::vec4(1, 0, 0, 1);
		gradient.data.data2 = glm::vec4(0, 0, 1, 1);

		VkPushConstantRange pushConstant{};
		pushConstant.offset = 0;
		pushConstant.size = sizeof(ComputePushConstants);
		pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo computeLayout{};
		computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout.pNext = nullptr;
		computeLayout.pSetLayouts = &m_DrawImageDescriptorLayout;
		computeLayout.setLayoutCount = 1;

		computeLayout.pPushConstantRanges = &pushConstant;
		computeLayout.pushConstantRangeCount = 1;

		VK_CHECK(vkCreatePipelineLayout(m_Device, &computeLayout, nullptr, &gradient.layout));

		// layout code
		VkShaderModule gradientShader;
		// TODO: Replace by a real shader loading pipeline with glslc and spirv cache loading.
		if (!Utils::LoadShaderModule("Assets/gradient.comp.spv", m_Device, &gradientShader)) {
			MGN_CORE_ERROR("[Vulkan] Error when building the compute shader '{}'", "Assets/gradient.comp.spv");
		}

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = gradientShader;
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = gradient.layout;
		computePipelineCreateInfo.stage = stageinfo;


		VK_CHECK(vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradient.pipeline));

		// add the 2 background effects into the array
		m_BackgroundEffects.push_back(gradient);
		// Not needed anymore.
		vkDestroyShaderModule(m_Device, gradientShader, nullptr);

		m_MainDeletionQueue.push(gradient.layout);
		m_MainDeletionQueue.push(gradient.pipeline);
	}

	void VulkanRenderer::InitSkyPipeline() {
		ComputeEffect sky;
		sky.layout = nullptr;
		sky.name = "sky";
		sky.data = {};
		// default sky parameters
		sky.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

		VkPushConstantRange pushConstant{};
		pushConstant.offset = 0;
		pushConstant.size = sizeof(ComputePushConstants);
		pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkPipelineLayoutCreateInfo computeLayout{};
		computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		computeLayout.pNext = nullptr;
		computeLayout.pSetLayouts = &m_DrawImageDescriptorLayout;
		computeLayout.setLayoutCount = 1;

		computeLayout.pPushConstantRanges = &pushConstant;
		computeLayout.pushConstantRangeCount = 1;

		VK_CHECK(vkCreatePipelineLayout(m_Device, &computeLayout, nullptr, &sky.layout));

		// layout code
		VkShaderModule skyShader;
		// TODO: Replace by a real shader loading pipeline with glslc and spirv cache loading.
		if (!Utils::LoadShaderModule("Assets/sky.comp.spv", m_Device, &skyShader)) {
			MGN_CORE_ERROR("[Vulkan] Error when building the compute shader '{}'", "Assets/sky.comp.spv");
		}

		VkPipelineShaderStageCreateInfo stageinfo{};
		stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageinfo.pNext = nullptr;
		stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageinfo.module = skyShader;
		stageinfo.pName = "main";

		VkComputePipelineCreateInfo computePipelineCreateInfo{};
		computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computePipelineCreateInfo.pNext = nullptr;
		computePipelineCreateInfo.layout = sky.layout;
		computePipelineCreateInfo.stage = stageinfo;


		VK_CHECK(vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.pipeline));

		// add the 2 background effects into the array
		m_BackgroundEffects.push_back(sky);
		// Not needed anymore.
		vkDestroyShaderModule(m_Device, skyShader, nullptr);

		m_MainDeletionQueue.push(sky.layout);
		m_MainDeletionQueue.push(sky.pipeline);
	}

	void VulkanRenderer::InitTrianglePipeline() {
		VkShaderModule triangleVertexShader{nullptr};
		if (!Utils::LoadShaderModule("Assets/colored_triangle.vert.spv", m_Device, &triangleVertexShader)) {
			MGN_CORE_ERROR("Error when building the triangle vertex shader module");
		}
		else {
			MGN_CORE_INFO("Triangle vertex shader succesfully loaded");
		}

		VkShaderModule triangleFragmentShader{nullptr};
		if (!Utils::LoadShaderModule("Assets/colored_triangle.frag.spv", m_Device, &triangleFragmentShader)) {
			MGN_CORE_ERROR("Error when building the triangle fragment shader module");
		}
		else {
			MGN_CORE_INFO("Triangle fragment shader succesfully loaded");
		}

		// build the pipeline layout that controls the inputs/outputs of the shader
		// we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = Initializer::PipelineLayoutCreateInfo();
		VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutCreateInfo, nullptr, &m_TrianglePipelineLayout));
		m_MainDeletionQueue.push(m_TrianglePipelineLayout);

		// connecting the vertex and pixel shaders to the pipeline
		m_TrianglePipeline = PipelineBuilder(m_TrianglePipelineLayout)
									 .AddShader(ShaderStage::Fragment, triangleFragmentShader)
									 .AddShader(ShaderStage::Vertex, triangleVertexShader)
									 .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
									 .SetPolygonMode(VK_POLYGON_MODE_FILL)
									 .SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
									 .SetMultisamplingNone()
									 .EnableBlendingAlpha()
									 .EnableDepthTest(true, VK_COMPARE_OP_LESS_OR_EQUAL)
									 .SetColorAttachmentFormat(m_DrawImage.imageFormat)
									 .SetDepthFormat(m_DepthImage.imageFormat)
									 .BuildPipeline(m_Device);

		m_MainDeletionQueue.push(m_TrianglePipeline);

		// clean structures
		vkDestroyShaderModule(m_Device, triangleFragmentShader, nullptr);
		vkDestroyShaderModule(m_Device, triangleVertexShader, nullptr);
	}
	void VulkanRenderer::InitMeshPipeline() {
		VkShaderModule triangleVertexShader{nullptr};
		if (!Utils::LoadShaderModule("Assets/colored_triangle_mesh.vert.spv", m_Device, &triangleVertexShader)) {
			MGN_CORE_ERROR("Error when building the mesh vertex shader module");
		}
		else {
			MGN_CORE_INFO("mesh vertex shader succesfully loaded");
		}

		VkShaderModule triangleFragmentShader{nullptr};
		if (!Utils::LoadShaderModule("Assets/tex_image.frag.spv", m_Device, &triangleFragmentShader)) {
			MGN_CORE_ERROR("Error when building the mesh fragment shader module");
		}
		else {
			MGN_CORE_INFO("mesh fragment shader succesfully loaded");
		}


		VkPushConstantRange bufferRange{};
		bufferRange.offset = 0;
		bufferRange.size = sizeof(GPUDrawPushConstants);
		bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		// build the pipeline layout that controls the inputs/outputs of the shader
		// we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = Initializer::PipelineLayoutCreateInfo();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &bufferRange;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &m_SingleImageDescriptorLayout;

		VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutCreateInfo, nullptr, &m_MeshPipelineLayout));
		m_MainDeletionQueue.push(m_MeshPipelineLayout);

		// connecting the vertex and pixel shaders to the pipeline
		m_MeshPipeline = PipelineBuilder(m_MeshPipelineLayout)
								 .AddShader(ShaderStage::Fragment, triangleFragmentShader)
								 .AddShader(ShaderStage::Vertex, triangleVertexShader)
								 .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
								 .SetPolygonMode(VK_POLYGON_MODE_FILL)
								 .SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
								 .SetMultisamplingNone()
								 .EnableBlendingAlpha()
								 .EnableDepthTest(true, VK_COMPARE_OP_LESS_OR_EQUAL)
								 .SetColorAttachmentFormat(m_DrawImage.imageFormat)
								 .SetDepthFormat(m_DepthImage.imageFormat)
								 .BuildPipeline(m_Device);

		m_MainDeletionQueue.push(m_MeshPipeline);

		// clean structures
		vkDestroyShaderModule(m_Device, triangleFragmentShader, nullptr);
		vkDestroyShaderModule(m_Device, triangleVertexShader, nullptr);
	}

	void VulkanRenderer::CreateSwapChain(const uint32_t width, const uint32_t height) {
		vkb::SwapchainBuilder swapchainBuilder{m_PhysicalDevice, m_Device, m_Surface};

		m_SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM; // TODO: See with HDR if we really want this one.

		vkb::Swapchain vkbSwapchain = swapchainBuilder
											  //.use_default_format_selection()
											  .set_desired_format(VkSurfaceFormatKHR{
													  .format = m_SwapchainImageFormat,
													  .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
											  // use vsync present mode
											  .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
											  // Hard VSync. Will limit the refreshrate to the one of the monitor.
											  .set_desired_extent(width, height)
											  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
											  .build()
											  .value();

		m_SwapchainExtent = vkbSwapchain.extent;

		// store swapchain and its related images
		m_Swapchain = vkbSwapchain.swapchain;
		m_SwapchainImages = vkbSwapchain.get_images().value();
		m_SwapchainImageViews = vkbSwapchain.get_image_views().value();
	}

	void VulkanRenderer::DestroySwapChain() {
		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
		m_Swapchain = nullptr;

		// destroy swapchain resources
		for (auto &swapchainImageView: m_SwapchainImageViews) {
			vkDestroyImageView(m_Device, swapchainImageView, nullptr);
		}
		m_SwapchainImages.clear();
		m_SwapchainImageViews.clear();
	}
	void VulkanRenderer::ResizeSwapChain() {
		vkDeviceWaitIdle(m_Device);

		DestroySwapChain();

		const auto size = Window::Get()->GetFramebufferSize();

		CreateSwapChain(size.width, size.height);

		m_ResizeRequested = false;
	}

	AllocatedBuffer VulkanRenderer::CreateBuffer(const uint64_t allocSize, const VkBufferUsageFlags usage, const VmaMemoryUsage memoryUsage) {
		// allocate buffer
		VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		bufferInfo.pNext = nullptr;
		bufferInfo.size = allocSize;

		bufferInfo.usage = usage;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = memoryUsage;
		vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		AllocatedBuffer newBuffer;

		// allocate the buffer
		VK_CHECK(vmaCreateBuffer(m_Allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation, &newBuffer.info));

		return newBuffer;
	}

	void VulkanRenderer::DestroyBuffer(AllocatedBuffer &buffer) {
		vmaDestroyBuffer(m_Allocator, buffer.buffer, buffer.allocation);
		buffer = AllocatedBuffer{}; // Reset the buffer to avoid access to deleted ptr.
	}

	GPUMeshBuffers VulkanRenderer::UploadMesh(ConstBufferView indices, ConstBufferView vertices) {
		const uint64_t vertexBufferSize = vertices.Size();
		const uint64_t indexBufferSize = indices.Size();

		GPUMeshBuffers GPUMesh;

		GPUMesh.vertexBuffer = CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		// find the adress of the vertex buffer
		VkBufferDeviceAddressInfo deviceAdressInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = GPUMesh.vertexBuffer.buffer};
		GPUMesh.vertexBufferAddress = vkGetBufferDeviceAddress(m_Device, &deviceAdressInfo);

		// create index buffer
		GPUMesh.indexBuffer = CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		AllocatedBuffer staging = CreateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void *data = staging.allocation->GetMappedData();

		// copy vertex buffer
		memcpy(data, vertices.Get(), vertexBufferSize);
		// copy index buffer
		memcpy((uint8_t *) data + vertexBufferSize, indices.Get(), indexBufferSize);

		ImmediateSubmit([&](VkCommandBuffer cmd) {
			VkBufferCopy vertexCopy{0};
			vertexCopy.dstOffset = 0;
			vertexCopy.srcOffset = 0;
			vertexCopy.size = vertexBufferSize;

			vkCmdCopyBuffer(cmd, staging.buffer, GPUMesh.vertexBuffer.buffer, 1, &vertexCopy);

			VkBufferCopy indexCopy{0};
			indexCopy.dstOffset = 0;
			indexCopy.srcOffset = vertexBufferSize;
			indexCopy.size = indexBufferSize;

			vkCmdCopyBuffer(cmd, staging.buffer, GPUMesh.indexBuffer.buffer, 1, &indexCopy);
		});

		DestroyBuffer(staging);
		return GPUMesh;
	}
	VkDescriptorSetLayout VulkanRenderer::GetGPUSceneDescriptorLayout() {
		return m_GpuSceneDataDescriptorLayout;
	}
	VkFormat VulkanRenderer::GetColorImageFormat() const {
		return m_DrawImage.imageFormat;
	}
	VkFormat VulkanRenderer::GetDepthImageFormat() const {
		return m_DepthImage.imageFormat;
	}

	void VulkanRenderer::CreateDefaultSamplers() {
		VkSamplerCreateInfo sampl = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
		sampl.magFilter = VK_FILTER_NEAREST;
		sampl.minFilter = VK_FILTER_NEAREST;

		vkCreateSampler(m_Device, &sampl, nullptr, &m_DefaultSamplerNearest);
		m_MainDeletionQueue.push(m_DefaultSamplerNearest);

		sampl.magFilter = VK_FILTER_LINEAR;
		sampl.minFilter = VK_FILTER_LINEAR;

		vkCreateSampler(m_Device, &sampl, nullptr, &m_DefaultSamplerLinear);
		m_MainDeletionQueue.push(m_DefaultSamplerLinear);
	}
	void VulkanRenderer::InitDefaultData() {

		// 3 default textures, white, grey, black. 1 pixel each
		uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
		m_WhiteImage = CreateImage(&white, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
		m_MainDeletionQueue.push(m_Allocator, m_WhiteImage.allocation, m_WhiteImage.image);
		m_MainDeletionQueue.push(m_WhiteImage.imageView);

		uint32_t grey = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
		m_GreyImage = CreateImage(&grey, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
		m_MainDeletionQueue.push(m_Allocator, m_GreyImage.allocation, m_GreyImage.image);
		m_MainDeletionQueue.push(m_GreyImage.imageView);

		uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
		m_BlackImage = CreateImage(&black, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
		m_MainDeletionQueue.push(m_Allocator, m_BlackImage.allocation, m_BlackImage.image);
		m_MainDeletionQueue.push(m_BlackImage.imageView);

		// checkerboard image
		uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
		std::array<uint32_t, 16 * 16> pixels; // for 16x16 checkerboard texture
		for (int x = 0; x < 16; x++) {
			for (int y = 0; y < 16; y++) {
				pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
			}
		}
		m_ErrorCheckerboardImage = CreateImage(pixels.data(), VkExtent3D{16, 16, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
		m_MainDeletionQueue.push(m_Allocator, m_ErrorCheckerboardImage.allocation, m_ErrorCheckerboardImage.image);
		m_MainDeletionQueue.push(m_ErrorCheckerboardImage.imageView);

		GLTFMetallicRoughness::MaterialResources materialResources;
		// default the material textures
		materialResources.colorImage = m_WhiteImage;
		materialResources.colorSampler = m_DefaultSamplerLinear;
		materialResources.metalImage = m_WhiteImage;
		materialResources.metalSampler = m_DefaultSamplerLinear;
		materialResources.roughImage = m_WhiteImage;
		materialResources.roughSampler = m_DefaultSamplerLinear;
		materialResources.normalImage = m_WhiteImage;
		materialResources.normalSampler = m_DefaultSamplerLinear;
		materialResources.aoImage = m_WhiteImage;
		materialResources.aoSampler = m_DefaultSamplerLinear;

		// set the uniform buffer for the material data
		AllocatedBuffer materialConstants = CreateBuffer(sizeof(GLTFMetallicRoughness::MaterialConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		// write the buffer
		GLTFMetallicRoughness::MaterialConstants *sceneUniformData = (GLTFMetallicRoughness::MaterialConstants *) materialConstants.allocation->GetMappedData();
		sceneUniformData->colorFactors = glm::vec4{1, 1, 1, 1};
		sceneUniformData->metal_factor = glm::vec4{1, 0, 0, 0};
		sceneUniformData->rough_factor = glm::vec4{0, 0.5, 0, 0};

		m_MainDeletionQueue.push(m_Allocator, materialConstants.allocation, materialConstants.buffer);

		materialResources.dataBuffer = materialConstants.buffer;
		materialResources.dataBufferOffset = 0;

		m_DefaultMeshMaterial = std::make_shared<VulkanMaterialInstance>(m_MetalRoughMaterial.WriteMaterial(m_Device, MaterialPass::MainColor, materialResources, m_GlobalDescriptorAllocator));
		m_DefaultLineMaterial = std::make_shared<VulkanMaterialInstance>(m_LineMetalRoughMaterial.WriteMaterial(m_Device, MaterialPass::MainColor, materialResources, m_GlobalDescriptorAllocator));
		// m_DefaultPointMaterial = std::make_shared<VulkanMaterialInstance>(m_PointMetalRoughMaterial.WriteMaterial(m_Device, MaterialPass::MainColor, materialResources, m_GlobalDescriptorAllocator));
	}

	AllocatedImage VulkanRenderer::CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {
		AllocatedImage img{};
		img.imageFormat = format;
		img.imageExtent = size;

		VkImageCreateInfo img_info = Initializer::ImageCreateInfo2D(format, usage, size);
		if (mipmapped) {
			img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
		}

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// allocate and create the image
		VK_CHECK(vmaCreateImage(m_Allocator, &img_info, &allocInfo, &img.image, &img.allocation, nullptr));

		// if the format is a depth format, we will need to have it use the correct
		// aspect flag
		VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		if (format == VK_FORMAT_D32_SFLOAT) {
			aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		// build a image-view for the image
		VkImageViewCreateInfo viewInfo = Initializer::ImageViewCreateInfo2D(format, img.image, aspectFlag);
		viewInfo.subresourceRange.levelCount = img_info.mipLevels;

		VK_CHECK(vkCreateImageView(m_Device, &viewInfo, nullptr, &img.imageView))

		return img;
	}
	AllocatedImage VulkanRenderer::CreateImage(const void *data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {
		size_t dataSize = size.depth * size.width * size.height * 4;
		AllocatedBuffer uploadbuffer = CreateBuffer(dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		memcpy(uploadbuffer.info.pMappedData, data, dataSize);

		AllocatedImage new_image = CreateImage(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

		ImmediateSubmit([&](VkCommandBuffer cmd) {
			Utils::TransitionImage(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = size;

			// copy the buffer into the image
			vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
								   &copyRegion);

			Utils::TransitionImage(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

		DestroyBuffer(uploadbuffer);

		return new_image;
	}
	void VulkanRenderer::DestroyImage(const AllocatedImage &img) {
		vkDestroyImageView(m_Device, img.imageView, nullptr);
		vmaDestroyImage(m_Allocator, img.image, img.allocation);
	}

	void VulkanRenderer::ShutdownVulkan() {
		for (auto &frame: m_Frames) {
			vkDestroyCommandPool(m_Device, frame.m_CommandPool, nullptr);

			// destroy sync objects
			vkDestroyFence(m_Device, frame.m_RenderFence, nullptr);
			vkDestroySemaphore(m_Device, frame.m_RenderSemaphore, nullptr);
			vkDestroySemaphore(m_Device, frame.m_SwapchainSemaphore, nullptr);

			frame.m_DeletionQueue.flush(m_Device);
		}
		m_Frames.clear();

		m_MainDeletionQueue.flush(m_Device);

		DestroySwapChain();

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
	}

	VulkanFrameData &VulkanRenderer::GetCurrentFrame() {
		return m_Frames[m_FrameIndex];
	}
	VulkanFrameData &VulkanRenderer::GetNextFrame() {
		return m_Frames[(m_FrameIndex + 1) % m_Frames.size()];
	}

	const Core::RendererParameters &VulkanRenderer::GetRenderParams() const {
		return m_AppParams.Renderer.value();
	}

	void VulkanRenderer::UpdateCache() {
		ViewMatrixCached = GetViewMatrix();
		ProjectionMatrixCached = GetProjectionMatrix();
		ViewProjectMatrixCached = ProjectionMatrixCached * ViewMatrixCached;
		InvViewProjectMatrixCached = glm::inverse(ViewProjectMatrixCached);
	}

	void VulkanRenderer::Resize() {
		vkDeviceWaitIdle(m_Device);
		ResizeSwapChain();
	}

	bool VulkanRenderer::BeginDraw() {
		m_MainDrawContext.OpaqueSurfaces.clear();

		// TODO: Preserve aspect ratio
		if (MgnImGui::DockingEnabled() && m_ImGuiViewport.has_value()) {
			auto size = m_ImGuiViewport.value().GetSize();
			m_DrawExtent.width = std::max(std::ceil(size.x * m_RenderScale), 1.f);
			m_DrawExtent.height = std::max(std::ceil(size.y * m_RenderScale), 1.f);
		}
		else {
			m_DrawExtent.width = std::ceil(std::min(m_SwapchainExtent.width, m_DrawImage.imageExtent.width) * m_RenderScale);
			m_DrawExtent.height = std::ceil(std::min(m_SwapchainExtent.height, m_DrawImage.imageExtent.height) * m_RenderScale);
		}

		// MGN_CORE_LOG_DEBUG("Swapchain is {}x{}",m_SwapchainExtent.width, m_SwapchainExtent.height);
		// MGN_CORE_LOG_DEBUG("Draw Size is {}x{}",m_DrawExtent.width, m_DrawExtent.height);

		const bool canDraw = m_DrawExtent.width > 0 && m_DrawExtent.height > 0;
		if (!canDraw) {
			return false;
		}

		// wait until the gpu has finished rendering the last frame. Timeout of 1
		// second
		VK_CHECK(vkWaitForFences(m_Device, 1, &GetCurrentFrame().m_RenderFence, true, 1000000000));
		VK_CHECK(vkResetFences(m_Device, 1, &GetCurrentFrame().m_RenderFence));

		GetCurrentFrame().m_DeletionQueue.flush(m_Device);
		GetCurrentFrame().m_FrameDescriptors.ClearPools(m_Device);

		// request image from the swapchain
		VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain, 1000000000, GetCurrentFrame().m_SwapchainSemaphore, nullptr, &GetCurrentFrame().m_SwapchainImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_ResizeRequested = true;
		}
		else {
			MGN_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failing to acquire the Swap Chain Image.");
		}

		VkCommandBuffer cmd{nullptr};
		cmd = GetCurrentFrame().m_MainCommandBuffer;
		VK_CHECK(vkResetCommandBuffer(cmd, 0));

		const auto beginInfo = Initializer::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

		UpdateCache();

		{
			m_SceneData.view = ViewMatrixCached;
			m_SceneData.proj = ProjectionMatrixCached;
			m_SceneData.viewproj = ViewProjectMatrixCached;

			// some default lighting parameters
			m_SceneData.ambientColor = glm::vec4(.1f);
			m_SceneData.sunlightColor = glm::vec4(1.f);
			m_SceneData.sunlightDirection = glm::vec4(0, 1, 0.5, 1.f);
		}

		return true;
	}
	void VulkanRenderer::EndDraw() {
		VkCommandBuffer cmd{nullptr};
		cmd = GetCurrentFrame().m_MainCommandBuffer;

		// vkCmdEndRendering(cmd);

		// Ending the drawing commands and copying the data into the swapchain image.
		Utils::TransitionImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		Utils::TransitionImage(cmd, m_SwapchainImages[GetCurrentFrame().m_SwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		Utils::CopyImageToImage(cmd, m_DrawImage.image, m_SwapchainImages[GetCurrentFrame().m_SwapchainImageIndex], m_DrawExtent, m_SwapchainExtent, VK_IMAGE_ASPECT_COLOR_BIT);

		// make the swapchain image into presentable mode
		Utils::TransitionImage(cmd, m_SwapchainImages[GetCurrentFrame().m_SwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		Utils::TransitionImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		DrawImGui(cmd, m_SwapchainImageViews[GetCurrentFrame().m_SwapchainImageIndex]);
		Utils::TransitionImage(cmd, m_SwapchainImages[GetCurrentFrame().m_SwapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);


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

		m_MainDrawContext.OpaqueSurfaces.clear();
	}
	void VulkanRenderer::Present() {
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

		presentInfo.pImageIndices = &GetCurrentFrame().m_SwapchainImageIndex;

		VkResult result = vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_ResizeRequested) {
			ResizeSwapChain();
		}
		else {
			MGN_CORE_ASSERT(result == VK_SUCCESS, "Failing to acquire the Swap Chain Image.");
		}
		m_FrameIndex = (m_FrameIndex + 1) % GetRenderParams().NbrFrameInFlight;
	}

	void VulkanRenderer::Draw() {
		VkCommandBuffer cmd{nullptr};
		cmd = GetCurrentFrame().m_MainCommandBuffer;

		// make the swapchain image into writeable mode before rendering
		// TODO: See if the layout VK_IMAGE_LAYOUT_GENERAL is the most optimal

		// Prepare the image for the drawing
		Utils::TransitionImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		// ALl the draw command starting with the clearing of the background
		DrawBackground(cmd);

		Utils::TransitionImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		Utils::TransitionImage(cmd, m_DepthImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

		DrawGeometry(cmd);
	}

	void VulkanRenderer::SendImGuiCommands() {
#ifdef MGN_IMGUI

		ImGui::SetNextWindowSize({400,600}, ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Background")) {

			ComputeEffect &selected = m_BackgroundEffects[m_CurrentBackgroundEffect];
			ImGui::DragFloat3("Camera Position", glm::value_ptr(Camera::s_MainCamera->position), 0.1, 0, 0, "%.1f");
			ImGui::DragFloat("Camera Speed", &Camera::s_MainCamera->velocityMultiplier, 0.1, 0, 0, "%.1f");

			ImGui::DragFloat("Camera Pitch", &Camera::s_MainCamera->pitch, 0.1, 0, 0, "%.1f");
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				Camera::s_MainCamera->pitch = std::fmodf(Camera::s_MainCamera->pitch, 360.f);
			}

			ImGui::DragFloat("Camera Yaw", &Camera::s_MainCamera->yaw, 0.1, 0, 0, "%.1f");
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				Camera::s_MainCamera->yaw = std::fmodf(Camera::s_MainCamera->yaw, 360.f);
			}

			ImGui::Spacing();
			ImGui::SliderFloat("Render Scale", &m_RenderScale, 0.3f, 1.f);

			ImGui::Text("Selected effect: ", selected.name);

			ImGui::SliderInt("Effect Index", &m_CurrentBackgroundEffect, 0, m_BackgroundEffects.size() - 1);

			ImGui::InputFloat4("data1", (float *) &selected.data.data1);
			ImGui::InputFloat4("data2", (float *) &selected.data.data2);
			ImGui::InputFloat4("data3", (float *) &selected.data.data3);
			ImGui::InputFloat4("data4", (float *) &selected.data.data4);
		}
		ImGui::End();


		ImGui::SetNextWindowSize({400,400}, ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Rendering")) {
			const ImVec2 pos = ImGui::GetCursorScreenPos();
			const ImVec2 size = ImGui::GetContentRegionAvail();

			const bool viewportFocused = ImGui::IsWindowFocused();
			// const bool viewportHovered = ImGui::IsWindowHovered();
			if (m_ViewportFocused != viewportFocused) {
				MgnImGui::SetEventsBlocked(!viewportFocused);
				m_ViewportFocused = viewportFocused;
				//TODO: Launch event for viewport focus changed.
			}

			m_ImGuiViewport = {pos.x, pos.y, pos.x + size.x, pos.y + size.y};

			ImGui::PushStyleVar(ImGuiStyleVar_ImageBorderSize, 0);
			ImGui::Image((ImTextureID) m_ImGuiImageDescriptors, {size.x, size.y}, {0.f, 0.f}, {(float) m_DrawExtent.width / (float) m_DrawImage.imageExtent.width, (float) m_DrawExtent.height / (float) m_DrawImage.imageExtent.height});
			ImGui::PopStyleVar(1);
		}
		ImGui::End();

#endif
	}

	void VulkanRenderer::LoadExternalModelInScene(const std::filesystem::path &path, Scene *scene, EntityID parent) {
		Initializer::LoadModelAsDynamic(this, scene, parent, path);
	}
	void VulkanRenderer::LoadCPUMeshInScene(const Core::CPUMesh &m, Core::Scene *scene, Core::EntityID entity) {
		std::shared_ptr<MeshAsset> GPUMesh = Initializer::LoadCPUMesh(this, m).value();
		if (!entity.IsValid()) {
			entity = scene->CreateEntity();
		}

		Renderable *render = scene->GetOrAddComponent<Renderable>(entity);
		if (!render) return;
		render->mesh = GPUMesh;
	}

	void VulkanRenderer::DrawBackground(VkCommandBuffer cmd) {
		//    	// make a clear-color from Time.
		//    	VkClearColorValue clearValue;
		//    	float flash = std::abs(std::sin(static_cast<float>(Application::Get()->Time())));
		//    	clearValue = {{0.0f, 0.0f, flash, 1.0f}};
		//
		//    	VkImageSubresourceRange clearRange = Initializer::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
		//
		//    	// clear image
		//    	vkCmdClearColorImage(cmd, m_DrawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

		ComputeEffect &effect = m_BackgroundEffects[m_CurrentBackgroundEffect];

		// bind the gradient drawing compute pipeline
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);

		// bind the descriptor set containing the draw image for the compute pipeline
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.layout, 0, 1, &m_DrawImageDescriptors, 0, nullptr);

		vkCmdPushConstants(cmd, effect.layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect.data);

		// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
		vkCmdDispatch(cmd, std::ceil(m_DrawExtent.width / 16.0), std::ceil(m_DrawExtent.height / 16.0), 1);
	}

	void VulkanRenderer::DrawGeometry(VkCommandBuffer cmd) {
	}

	void VulkanRenderer::Draw(const Core::DrawContext &ctx) {
		VkCommandBuffer cmd{nullptr};
		cmd = GetCurrentFrame().m_MainCommandBuffer;

		std::vector<ManualDeleteMeshAsset> lineMeshes;
		ManualDeleteMeshAsset pointMesh;
		if (!ctx.OpaqueLines.empty()) {
			for (const LineObject & line: ctx.OpaqueLines) {
				lineMeshes.emplace_back(std::move(Initializer::LoadManualLines(this, {(LineObject*)&line, 1})));
				PushCurrentFrameDeletion(lineMeshes.back().meshBuffers.indexBuffer.allocation, lineMeshes.back().meshBuffers.indexBuffer.buffer);
				PushCurrentFrameDeletion(lineMeshes.back().meshBuffers.vertexBuffer.allocation, lineMeshes.back().meshBuffers.vertexBuffer.buffer);
			}
		}
		// TODO: Implement a point renderer when it's ready. Like, by doing a Geometry shader or some things.
		// if (!ctx.OpaquePoints.empty()) {
		// 	pointMesh = Initializer::LoadPoints(this, (std::vector<Vertex> &) (ctx.OpaquePoints));
		// }

		VkRenderingAttachmentInfo colorAttachment = Initializer::RenderingAttachmentInfo(m_DrawImage.imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		VkRenderingAttachmentInfo depthAttachment = Initializer::DepthAttachmentInfo(m_DepthImage.imageView, 1.0f, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

		VkRenderingInfo renderInfo = Initializer::RenderingInfo(m_DrawExtent, &colorAttachment, &depthAttachment);
		vkCmdBeginRendering(cmd, &renderInfo);

		// set dynamic viewport and scissor
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = m_DrawExtent.width;
		viewport.height = m_DrawExtent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_DrawExtent.width;
		scissor.extent.height = m_DrawExtent.height;

		vkCmdSetScissor(cmd, 0, 1, &scissor);

		// allocate a new uniform buffer for the scene data
		// using CPU_TO_GPU as the `GPUSceneData` type is very small and can probably be cached entirely in VRAM.
		// better use a dedicated `VMA_MEMORY_USAGE_GPU` for bigger data and do a transfer beforehand.
		AllocatedBuffer gpuSceneDataBuffer = CreateBuffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		// add it to the deletion queue of this frame so it gets deleted once its been used
		GetCurrentFrame().m_DeletionQueue.push(Deleter::VmaBuffer{m_Allocator, gpuSceneDataBuffer.allocation, gpuSceneDataBuffer.buffer});

		// write the buffer
		GPUSceneData *sceneUniformData = (GPUSceneData *) gpuSceneDataBuffer.allocation->GetMappedData();
		*sceneUniformData = m_SceneData;

		// create a descriptor set that binds that buffer and update it
		GetCurrentFrame().m_GlobalDescriptor = GetCurrentFrame().m_FrameDescriptors.Allocate(m_Device, m_GpuSceneDataDescriptorLayout);

		DescriptorWriter writer;
		writer.WriteBuffer(0, gpuSceneDataBuffer.buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.UpdateSet(m_Device, GetCurrentFrame().m_GlobalDescriptor);

		for (const RenderObject &draw: ctx.OpaqueSurfaces) {

			MeshAsset *mesh = dynamic_cast<MeshAsset *>(draw.mesh.get());

			MGN_CORE_ASSERT(mesh, "The mesh is not a valid vulkan mesh.");
			// TODO: Do some smart LOD selection instead of the best one everytime
			const Mesh::LOD &lod = draw.mesh->lods.front();

			const VulkanMaterialInstance *material = dynamic_cast<const VulkanMaterialInstance *>(lod.material.get());

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipeline);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->layout, 0, 1, &GetCurrentFrame().m_GlobalDescriptor, 0, nullptr);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->layout, 1, 1, &material->materialSet, 0, nullptr);

			vkCmdBindIndexBuffer(cmd, mesh->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

			GPUDrawPushConstants pushConstants;
			pushConstants.vertexBuffer = mesh->meshBuffers.vertexBufferAddress;
			pushConstants.worldMatrix = draw.transform;
			vkCmdPushConstants(cmd, material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);

			vkCmdDrawIndexed(cmd, lod.count, 1, lod.index, 0, 0);
		}

		for (uint64_t i = 0; i < lineMeshes.size(); ++i) {
			ManualDeleteMeshAsset *mesh = &lineMeshes[i];

			MGN_CORE_ASSERT(mesh, "The mesh is not a valid vulkan mesh.");
			// TODO: Do some smart LOD selection instead of the best one everytime
			const Mesh::LOD &lod = mesh->lods.front();

			const VulkanMaterialInstance *material = dynamic_cast<const VulkanMaterialInstance *>(lod.material.get());
			// vkCmdSetLineWidth(cmd, ctx.OpaqueLines[i].width);

			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipeline);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->layout, 0, 1, &GetCurrentFrame().m_GlobalDescriptor, 0, nullptr);
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->layout, 1, 1, &material->materialSet, 0, nullptr);

			vkCmdBindIndexBuffer(cmd, mesh->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

			GPUDrawPushConstants pushConstants;
			pushConstants.vertexBuffer = mesh->meshBuffers.vertexBufferAddress;
			pushConstants.worldMatrix = Math::Identity<glm::mat4>();
			vkCmdPushConstants(cmd, material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);

			vkCmdDrawIndexed(cmd, lod.count, 1, lod.index, 0, 0);
		}

		// if (pointMesh) {
		//
		// 	MeshAsset *mesh = pointMesh.get();
		//
		// 	MGN_CORE_ASSERT(mesh, "The mesh is not a valid vulkan mesh.");
		// 	// TODO: Do some smart LOD selection instead of the best one everytime
		// 	const Mesh::LOD &lod = mesh->lods.front();
		//
		// 	const VulkanMaterialInstance *material = dynamic_cast<const VulkanMaterialInstance *>(lod.material.get());
		//
		// 	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipeline);
		// 	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->layout, 0, 1, &GetCurrentFrame().m_GlobalDescriptor, 0, nullptr);
		// 	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->layout, 1, 1, &material->materialSet, 0, nullptr);
		//
		// 	vkCmdBindIndexBuffer(cmd, mesh->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
		//
		// 	GPUDrawPushConstants pushConstants;
		// 	pushConstants.vertexBuffer = mesh->meshBuffers.vertexBufferAddress;
		// 	pushConstants.worldMatrix = Math::Identity<glm::mat4>();
		// 	vkCmdPushConstants(cmd, material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);
		//
		// 	vkCmdDrawIndexed(cmd, lod.count, 1, lod.index, 0, 0);
		// }

		vkCmdEndRendering(cmd);
	}

	void VulkanRenderer::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)> &&function) {

		VK_CHECK(vkResetFences(m_Device, 1, &m_ImmFence));
		VK_CHECK(vkResetCommandBuffer(m_ImmCommandBuffer, 0));

		VkCommandBuffer cmd = m_ImmCommandBuffer;

		VkCommandBufferBeginInfo cmdBeginInfo = Initializer::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

		function(cmd);

		VK_CHECK(vkEndCommandBuffer(cmd));

		VkCommandBufferSubmitInfo cmdinfo = Initializer::CommandBufferSubmitInfo(cmd);
		VkSubmitInfo2 submit = Initializer::SubmitInfo(&cmdinfo, nullptr, nullptr);

		// submit command buffer to the queue and execute it.
		//  _renderFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit2(m_GraphicsQueue, 1, &submit, m_ImmFence));

		VK_CHECK(vkWaitForFences(m_Device, 1, &m_ImmFence, true, 9999999999));
	}

	void VulkanRenderer::InitializeImGui() {
#ifdef MGN_IMGUI
		// 1: create descriptor pool for IMGUI
		//  the size of the pool is very oversize, but it's copied from imgui demo
		//  itself.
		VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
											 {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
											 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
											 {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
											 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
											 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
											 {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = (uint32_t) std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		VkDescriptorPool imguiPool;
		VK_CHECK(vkCreateDescriptorPool(m_Device, &pool_info, nullptr, &imguiPool));

		// this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo init_info = {};
		// init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
		init_info.Instance = m_Instance;
		init_info.PhysicalDevice = m_PhysicalDevice;
		init_info.Device = m_Device;
		init_info.QueueFamily = m_GraphicsQueueFamily;
		init_info.Queue = m_GraphicsQueue;

		// init_info.PipelineCache = g_PipelineCache;
		// init_info.RenderPass = wd->RenderPass;
		// init_info.Subpass = 0;
		// init_info.Allocator = g_Allocator;
		init_info.CheckVkResultFn = [](const VkResult error) { VK_CHECK(error); };

		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.UseDynamicRendering = true;

		// dynamic rendering parameters for imgui to use
		init_info.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
		init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_SwapchainImageFormat;


		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&init_info);
		ImGui_ImplVulkan_CreateFontsTexture();

		m_MainDeletionQueue.push(imguiPool);


		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

		VkExtent2D maxDrawImage{deviceProperties.limits.maxImageDimension2D, deviceProperties.limits.maxImageDimension2D};

#endif
	}

	Core::DrawContext &VulkanRenderer::GetDrawContext() {
		return m_MainDrawContext;
	}

	void VulkanRenderer::DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView) {
#ifdef MGN_IMGUI
		VkRenderingAttachmentInfo colorAttachment = Initializer::RenderingAttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		VkRenderingInfo renderInfo = Initializer::RenderingInfo(m_SwapchainExtent, &colorAttachment, nullptr);

		vkCmdBeginRendering(cmd, &renderInfo);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		vkCmdEndRendering(cmd);
#endif
	}

} // namespace Imagine::Vulkan
