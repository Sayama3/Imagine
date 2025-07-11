//
// Created by Sayama on 28/05/2025.
//

#include <imgui_impl_vulkan.h>
#include "Imagine/ThirdParty/ImGui.hpp"
#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine::Vulkan;

namespace Imagine::ThirdParty::ImGuiLib {
	void InitializeRenderer() {

		VulkanRenderer *renderer = dynamic_cast<VulkanRenderer *>(Renderer::Get());
		MGN_CORE_CASSERT(renderer, "Vulkan Renderer not initialized.");
		renderer->InitializeImGui();
	}

	void NewRenderFrame() {
		ImGui_ImplVulkan_NewFrame();
	}

	void PostRender(){
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ShutdownRenderer() {
		ImGui_ImplVulkan_Shutdown();
	}
} // namespace Imagine
