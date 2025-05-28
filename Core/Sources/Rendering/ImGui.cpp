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
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
	}
	void MgnImGui::NewFrame(){
		ImGui::NewFrame();
	}
	void MgnImGui::Shutdown(){
		ImGui::DestroyContext();
	}
#else
	void MgnImGui::CreateContext() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::InitializeWindow() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::InitializeRenderer() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::NewFrame() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::NewRenderFrame() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::NewWindowFrame() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::WindowProcessEvents(void* event) {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::Render() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::Shutdown() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::ShutdownWindow() {MGN_LOG_ERROR("ImGui Not Implemented.");}
	void MgnImGui::ShutdownRenderer() {MGN_LOG_ERROR("ImGui Not Implemented.");}
#endif
} // namespace Imagine::Core
