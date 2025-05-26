//
// Created by Sayama on 23/05/2025.
//


#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Core/FileSystem.hpp"

using namespace Imagine::Core;

namespace Imagine::Vulkan {

	VulkanRenderer::VulkanRenderer(const RendererParameter& renderParams, const ApplicationParameters& appParams) : Renderer(), m_Parameters(renderParams)
	{
	}

	VulkanRenderer::~VulkanRenderer() {}

	void VulkanRenderer::InitialiseVulkan() {
	}

	void VulkanRenderer::CleanupVulkan() {
	}

	void VulkanRenderer::CreateInstance() {
		// ==================== Validating Vulkan Drivers ====================
		if (m_Parameters.EnableDebug) {
			MGN_CORE_ASSERT(CheckValidationLayerSupport(), "validation layers requested, but not available!");
		}

		vk::ApplicationInfo appInfo {
		};
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
