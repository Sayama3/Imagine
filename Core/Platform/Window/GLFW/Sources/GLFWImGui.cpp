//
// Created by Sayama on 28/05/2025.
//


#include <imgui_impl_glfw.h>
#include "GLFWWindow.hpp"
#include "Imagine/Application/Window.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Rendering/MgnImGui.hpp"

namespace Imagine {
	void MgnImGui::InitializeWindow() {
		MGN_CORE_CASSERT(Imagine::Window::Get(), "Window is not created.");
#ifdef MGN_RENDERER_VULKAN
		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow *>(Imagine::Window::Get()->GetWindowPtr()), true);
#else
#error "The renderer is not implemented for ImGui"
#endif
	}

	void MgnImGui::NewWindowFrame() {
		ImGui_ImplGlfw_NewFrame();
	}

	void MgnImGui::ShutdownWindow() {
		ImGui_ImplGlfw_Shutdown();
	}
} // namespace Imagine
