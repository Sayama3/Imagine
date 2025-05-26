//
// Created by Sayama on 23/05/2025.
//

#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>
#include <vector>

#include "Imagine/Rendering/Renderer.hpp"

namespace Imagine::Vulkan {
	class VulkanRenderer final : public Core::Renderer {
	public:
		inline static constexpr uint16_t MAX_FRAMES_IN_FLIGHT = 2;
		inline static const std::vector<const char *> c_ValidationLayers {"VK_LAYER_KHRONOS_validation",};
		inline static const std::vector<const char*> c_DeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

public:
		VulkanRenderer(const Core::RendererParameter& renderParams, const Core::ApplicationParameters& appParams);
		virtual ~VulkanRenderer() override;
public:
private:
		void InitialiseVulkan();
		void CleanupVulkan();
private:
		void CreateInstance();
		void SetupDebugger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
private:
		bool CheckValidationLayerSupport();
private:
		VkInstance m_Instance;// Vulkan library handle
		VkDebugUtilsMessengerEXT m_DebugMessenger;// Vulkan debug output handle
		VkPhysicalDevice m_ChosenGPU;// GPU chosen as the default device
		VkDevice m_Device; // Vulkan device for commands
		VkSurfaceKHR m_Surface;// Vulkan window surface

		Core::RendererParameter m_Parameters;
	};
}