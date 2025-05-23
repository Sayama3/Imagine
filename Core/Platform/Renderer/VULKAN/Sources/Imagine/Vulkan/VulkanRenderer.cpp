//
// Created by Sayama on 23/05/2025.
//


#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Core/FileSystem.hpp"

using namespace Imagine::Core;

namespace Imagine::Vulkan {

	VulkanRenderer::VulkanRenderer() : Renderer() {}

	VulkanRenderer::~VulkanRenderer() {}

	void VulkanRenderer::InitialiseVulkan() {
	}

	void VulkanRenderer::CleanupVulkan() {
	}

	void VulkanRenderer::CreateInstance() {
		// ==================== Validating Vulkan Drivers ====================
		if constexpr (c_EnableValidationLayers) {
			MGN_CORE_ASSERT(CheckValidationLayerSupport(), "validation layers requested, but not available!");
		}
	}

	void VulkanRenderer::SetupDebugger() {
	}

	void VulkanRenderer::CreateSurface() {
	}

	void VulkanRenderer::PickPhysicalDevice() {
	}

	void VulkanRenderer::CreateLogicalDevice() {
	}

	bool VulkanRenderer::CheckValidationLayerSupport() {
		std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

		for (const char *layerName: c_ValidationLayers) {
			bool layerFound = false;

			for (const auto &layerProperties: availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}
}
