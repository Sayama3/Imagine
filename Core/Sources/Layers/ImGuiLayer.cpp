//
// Created by ianpo on 29/06/2025.
//

#include "Imagine/Layers/ImGuiLayer.hpp"
#include "Imagine/Application/Application.hpp"
#include "Imagine/Events/ApplicationEvent.hpp"
#include "Imagine/Rendering/MgnImGui.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#endif

namespace Imagine::Core {
	ImGuiLayer::~ImGuiLayer() {
	}

#ifdef MGN_IMGUI

	void ImGuiLayer::OnEvent(Event &e) {
		EventDispatcher dispatch(e);

		dispatch.Dispatch<ImGuiEvent>(MGN_BIND_EVENT_FN(ImGuiLayer::OnImGui));
		dispatch.Dispatch<WindowResizeEvent>(MGN_BIND_EVENT_FN(ImGuiLayer::OnResize));

		if (MgnImGui::EventsBlocked()) {
			const ImGuiIO &io = ImGui::GetIO();
			e.m_Handled |= e.IsInCategory(EventCategory::EventCategoryMouse) & io.WantCaptureMouse;
			e.m_Handled |= e.IsInCategory(EventCategory::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
	bool ImGuiLayer::OnResize(WindowResizeEvent &e) {
		return false;
	}

	bool ImGuiLayer::OnImGui(ImGuiEvent &e) {
		if (MgnImGui::DockingEnabled()) {
			RenderImGuiDockspace();
		}
#ifdef MGN_DEBUG
		static bool s_Open{true};
		if (s_Open) ImGui::ShowDemoWindow(&s_Open);
#endif

		return false;
	}
	void ImGuiLayer::RenderImGuiDockspace() {

		static constexpr ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		static constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
														 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
														 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		{
			const ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Imagine DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar();
		{
			// ImGui::Begin("Imagine Dockspace", &s_DockingEnabled, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground);
			ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

			ImGui::PopStyleVar(2);

			ImGuiID dockspace_id = ImGui::GetID("Imagine_Dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			RenderImGuiMenuBar();
		}

		ImGui::End();
	}

	void ImGuiLayer::RenderImGuiMenuBar() {

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Options")) {

				if (ImGui::MenuItem("Close")) {
					Application::Get()->Stop();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}
#else
	void ImGuiLayer::OnEvent(Event &event) {}
	void ImGuiLayer::OnImGui(ImGuiEvent &e) {}
	void ImGuiLayer::RenderImGuiDockspace() {}
	void ImGuiLayer::RenderImGuiMenuBar() {}
	bool ImGuiLayer::OnResize(WindowResizeEvent &e) {}
#endif
} // namespace Imagine::Core
