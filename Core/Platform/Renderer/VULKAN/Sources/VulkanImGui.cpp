//
// Created by Sayama on 28/05/2025.
//

#include <imgui_impl_vulkan.h>
#include "Imagine/Rendering/MgnImGui.hpp"
#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine::Vulkan;

namespace Imagine::Core {
	void MgnImGui::InitializeRenderer() {

		VulkanRenderer *renderer = dynamic_cast<VulkanRenderer *>(Renderer::Get());
		MGN_CORE_ASSERT(renderer, "Vulkan Renderer not initialized.");
		renderer->InitializeImGui();
	}

	void MgnImGui::NewRenderFrame() {
		ImGui_ImplVulkan_NewFrame();
	}

	void MgnImGui::Render(){
		ImGui::Render();
	}

	void MgnImGui::ShutdownRenderer(){
		ImGui_ImplVulkan_Shutdown();
	}
} // namespace Imagine::Core
