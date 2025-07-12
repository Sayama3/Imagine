//
// Created by ianpo on 11/07/2025.
//

#include "Imagine/Layers/ProjectLayer.hpp"
#include "Imagine/Project/Project.hpp"
#include "Imagine/Assets/AssetManager.hpp"
#include "Imagine/Assets/FileAssetManager.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine {
	void ProjectLayer::OnAttach() {
		Layer::OnAttach();
	}
	void ProjectLayer::OnDetach() {
		Layer::OnDetach();
	}
	void ProjectLayer::OnEvent(Event &event) {
		EventDispatcher dispatcher{event};
		dispatcher.Dispatch<ImGuiEvent>(MGN_DISPATCH_FALSE(OnImGui));
	}

	void ProjectLayer::OnImGui(ImGuiEvent &e) {
#ifdef MGN_IMGUI
		ImGuiProject();
		ImGuiAssetManager();
#endif
	}
	void ProjectLayer::ImGuiProject() {
#ifdef MGN_IMGUI
		Project* project = Project::GetActive();
		if (!project) return;


		ImGui::SetNextWindowSize({400,200}, ImGuiCond_FirstUseEver);
		ImGui::Begin("Project");
		{
			// project->m_Config
		}
		ImGui::End();
#endif
	}
	void ProjectLayer::ImGuiAssetManager() {
#ifdef MGN_IMGUI
		Project* project = Project::GetActive();
		if (!project) return;
		FileAssetManager* manager = project->GetFileAssetManager();
		if (!manager) return;

		ImGui::SetNextWindowSize({400,200}, ImGuiCond_FirstUseEver);
		ImGui::Begin("Asset Manager");
		{
			// ImGui::Tab
			// manager->m_LoadedAssets
			// manager->m_MemoryAssets
			// manager->m_AssetRegistry
		}
		ImGui::End();
#endif
	}
} // Imagine