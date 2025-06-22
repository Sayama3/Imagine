//
// Created by Sayama on 28/05/2025.
//

#include "Imagine/Rendering/MgnImGui.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#endif

namespace Imagine::Core {

#ifdef MGN_IMGUI
	void MgnImGui::CreateContext() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows


		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle &style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}
	void MgnImGui::NewFrame() {
		ImGui::NewFrame();
	}
	void MgnImGui::Shutdown() {
		ImGui::DestroyContext();
	}
#else
	void MgnImGui::CreateContext() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::InitializeWindow() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::InitializeRenderer() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::NewFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::NewRenderFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::NewWindowFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::Render() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::Shutdown() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::ShutdownWindow() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::ShutdownRenderer() { MGN_LOG_ERROR("ImGui Not Implemented."); }
#endif
} // namespace Imagine::Core
