//
// Created by ianpo on 11/07/2025.
//

#include "Imagine/ThirdParty/ImGui.hpp"
#include "Imagine/Core/InternalCore.hpp"

#ifdef MGN_IMGUI
#include <imgui_internal.h>
#endif


namespace Imagine::ThirdParty::ImGuiLib {
#ifdef MGN_IMGUI
	inline static bool s_DockingEnabled{false};
	inline static bool s_EventBlocked{true};
	inline static bool s_IsDarkTheme{true};

	void CreateContext() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows


		// Setup Dear ImGui style
		SetDarkThemeColors();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle &style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	void SetDarkThemeColors() {
		s_IsDarkTheme = true;
		ImGui::StyleColorsDark();
		ImGuiStyle &style = ImGui::GetStyle();
		auto &colors = style.Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

		// Headers
		colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
		colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
		colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
	}
	void SetLightThemeColors() {
		s_IsDarkTheme = false;
		ImGui::StyleColorsLight();
		ImGuiStyle &style = ImGui::GetStyle();
		auto &colors = style.Colors;
		// TODO: Set the colors lights.
	}
	void EnableDocking() {
		s_DockingEnabled = true;
	}

	void DisableDocking() {
		s_DockingEnabled = false;
	}

	void NewFrame() {
		ImGui::NewFrame();
	}

	void Render() {
		ImGui::Render();
	}

	bool DockingEnabled() {
		return s_DockingEnabled;
	}

	void Shutdown() {
		ImGui::DestroyContext();
	}

	void BlockEvents() {
		SetEventsBlocked(true);
	}
	void UnblockEvents() {
		SetEventsBlocked(false);
	}
	void SetEventsBlocked(const bool eventBlocked) {
		s_EventBlocked = eventBlocked;
	}
	bool EventsBlocked() {
		return s_EventBlocked;
	}

	bool DragReal(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, ImGuiSliderFlags flags) {
		return ImGui::DragScalarN(label, ImGuiDataType_Real, v, 1, v_speed, &v_min, &v_max, format, flags);
	}
	bool DragReal2(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, ImGuiSliderFlags flags) {
		return ImGui::DragScalarN(label, ImGuiDataType_Real, v, 2, v_speed, &v_min, &v_max, format, flags);
	}
	bool DragReal3(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, ImGuiSliderFlags flags) {
		return ImGui::DragScalarN(label, ImGuiDataType_Real, v, 3, v_speed, &v_min, &v_max, format, flags);
	}
	bool DragReal4(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, ImGuiSliderFlags flags) {
		return ImGui::DragScalarN(label, ImGuiDataType_Real, v, 4, v_speed, &v_min, &v_max, format, flags);
	}
	bool SliderReal(const char *label, Real *v, Real v_min, Real v_max, const char *format, ImGuiSliderFlags flags) {
		return ImGui::SliderScalar(label, ImGuiDataType_Real, v, &v_min, &v_max, format, flags);
	}

	bool InputUUID(const char *name, UUID *id, ImGuiInputTextFlags flags) {
		return ImGui::InputScalarN(name, ImGuiDataType_U64, id, UUID::Count64, nullptr, nullptr, "%llu", flags);
	}

#else
	bool InputUUID(const char *name, UUID *id, ImGuiInputTextFlags flags) {
		return false;
	}
	bool SliderReal(const char *label, Real *v, Real v_min, Real v_max, const char *format, ImGuiSliderFlags flags) {
		return false;
	}
	bool DockingEnabled() {
		MGN_LOG_ERROR("ImGui Not Implemented.");
		return false;
	}

	bool DragReal(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, int flags) { return false; }
	bool DragReal2(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, int flags) { return false; }
	bool DragReal3(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, int flags) { return false; }
	bool DragReal4(const char *label, Real *v, float v_speed, Real v_min, Real v_max, const char *format, int flags) { return false; }

	void CreateContext() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void InitializeWindow() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void InitializeRenderer() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void EnableDocking() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void DisableDocking() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void NewFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void NewRenderFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void NewWindowFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void Render() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void Shutdown() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void ShutdownWindow() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void ShutdownRenderer() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void BlockEvents() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void UnblockEvents() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	bool EventsBlocked() {
		MGN_LOG_ERROR("ImGui Not Implemented.");
		return false;
	}
	void SetEventsBlocked() { MGN_LOG_ERROR("ImGui Not Implemented."); }
#endif
} // namespace Imagine::ThirdParty::ImGuiLib
