//
// Created by Sayama on 28/05/2025.
//

#include "Imagine/Application/Application.hpp"
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
		SetDarkThemeColors();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle &style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	void MgnImGui::SetDarkThemeColors() {
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
	void MgnImGui::SetLightThemeColors() {
		s_IsDarkTheme = false;
		ImGui::StyleColorsLight();
		ImGuiStyle &style = ImGui::GetStyle();
		auto &colors = style.Colors;
		// TODO: Set the colors lights.
	}
	void MgnImGui::EnableDocking() {
		s_DockingEnabled = true;
	}

	void MgnImGui::DisableDocking() {
		s_DockingEnabled = false;
	}

	void MgnImGui::NewFrame() {
		ImGui::NewFrame();

		if (s_DockingEnabled) {
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
			static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

			{
				const ImGuiViewport *viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Imagine DockSpace", nullptr, window_flags);
			ImGui::PopStyleVar();

			// ImGui::Begin("Imagine Dockspace", &s_DockingEnabled, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground);
			ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

			ImGui::PopStyleVar(2);

			ImGuiID dockspace_id = ImGui::GetID("Imagine Dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("Options")) {

					if (ImGui::MenuItem("Close")) {
						Application::Get()->Stop();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::End();
		}
	}

	void MgnImGui::Render() {
		ImGui::Render();
	}

	bool MgnImGui::DockingEnabled() {
		return s_DockingEnabled;
	}

	void MgnImGui::Shutdown() {
		ImGui::DestroyContext();
	}

#else

	bool MgnImGui::DockingEnabled() {
		MGN_LOG_ERROR("ImGui Not Implemented.");
		return false;
	}
	void MgnImGui::CreateContext() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::InitializeWindow() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::InitializeRenderer() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::EnableDocking() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::DisableDocking() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::NewFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::NewRenderFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::NewWindowFrame() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::Render() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::Shutdown() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::ShutdownWindow() { MGN_LOG_ERROR("ImGui Not Implemented."); }
	void MgnImGui::ShutdownRenderer() { MGN_LOG_ERROR("ImGui Not Implemented."); }
#endif
} // namespace Imagine::Core
