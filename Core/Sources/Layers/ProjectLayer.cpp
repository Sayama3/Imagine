//
// Created by ianpo on 11/07/2025.
//

#include "Imagine/Layers/ProjectLayer.hpp"

#include "Imagine/Assets/AssetImporter.hpp"
#include "Imagine/Assets/AssetManager.hpp"
#include "Imagine/Assets/FileAssetManager.hpp"

#include "Imagine/Project/Project.hpp"
#include "Imagine/Project/ProjectSerializer.hpp"

#include "Imagine/Scene/Scene.hpp"

#include "Imagine/ThirdParty/FileDialogs.hpp"
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
		Project *project = Project::GetActive();
		if (!project) return;


		ImGui::SetNextWindowSize({400, 200}, ImGuiCond_FirstUseEver);
		ImGui::Begin("Project");
		{

			if (ImGui::Button("Save")) {
				Project::SaveActive();
			}

			ImGui::SameLine();
			static std::string projectPathStr = project->m_ProjectPath.string();
			static std::string assetDirectoryStr = project->m_Config.assetDirectory.string();
			static std::string cacheDirectoryStr = project->m_Config.cacheDirectory.string();
			static std::string scriptsDirectoryStr = project->m_Config.scriptsDirectory.string();
			static std::string assetRegistryPathStr = project->m_Config.assetRegistryPath.string();

			if (ImGui::Button("Load")) {
				const auto result = ThirdParty::FileDialogs::OpenFile("Load Project", project->GetFilePath().string());
				if (!result.empty() && !result[0].empty()) {
					auto new_project = Project::Load(result[0]);
					if (new_project) {
						project = new_project;
						projectPathStr = project->m_ProjectPath.string();
						assetDirectoryStr = project->m_Config.assetDirectory.string();
						cacheDirectoryStr = project->m_Config.cacheDirectory.string();
						scriptsDirectoryStr = project->m_Config.scriptsDirectory.string();
						assetRegistryPathStr = project->m_Config.assetRegistryPath.string();
					}
				}
			}

			ImGui::InputText("Name", &project->m_Config.name);

			ImGui::InputText("Project Path", &projectPathStr);
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				project->m_ProjectPath = projectPathStr;
			}

			ImGui::InputText("Assets Directory", &assetDirectoryStr);
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				project->m_Config.assetDirectory = assetDirectoryStr;
			}

			ImGui::InputText("Cache Directory", &cacheDirectoryStr);
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				project->m_Config.cacheDirectory = cacheDirectoryStr;
			}

			ImGui::InputText("Scripts Directory", &scriptsDirectoryStr);
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				project->m_Config.scriptsDirectory = scriptsDirectoryStr;
			}

			ImGui::InputText("Asset Registry Path", &assetRegistryPathStr);
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				project->m_Config.assetRegistryPath = assetRegistryPathStr;
			}

			ThirdParty::ImGuiLib::DrawAssetField<Scene>("Start Scene", &project->m_Config.startSceneId);
		}
		ImGui::End();
#endif
	}

	void ProjectLayer::ImGuiAssetManager() {
#ifdef MGN_IMGUI
		Project *project = Project::GetActive();
		if (!project) return;
		FileAssetManager *manager = project->GetFileAssetManager();
		if (!manager) return;

		ImGui::SetNextWindowSize({400, 200}, ImGuiCond_FirstUseEver);
		ImGui::Begin("Asset Manager");
		{
			static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg;
			if (ImGui::BeginTabBar("Assets Components", tab_bar_flags)) {
				if (ImGui::BeginTabItem("Asset Registry")) {
					static AssetHandle selected = NULL_ASSET_HANDLE;
					if (ImGui::BeginTable("AssetRegistryTable", 4, flags)) {
						// Display headers so we can inspect their interaction with borders
						// (Headers are not the main purpose of this section of the demo, so we are not elaborating on them now. See other sections for details)
						{
							ImGui::TableSetupColumn("Handle");
							ImGui::TableSetupColumn("Type");
							ImGui::TableSetupColumn("Path");
							ImGui::TableSetupColumn("Loaded");
							ImGui::TableHeadersRow();
						}

						for (auto &[id, metadata]: manager->m_AssetRegistry) {
							ImGui::TableNextRow();
							if (ImGui::TableSetColumnIndex(0)) {
								const auto idStr = metadata.Handle.GetID().raw_string();
								if (ImGui::Selectable(idStr.c_str(), selected == metadata.Handle)) selected = metadata.Handle;
								if (ImGui::BeginDragDropSource()) {
									std::string payloadID = AssetTypeToPayloadID(metadata.Type);
									MGN_CORE_CASSERT(payloadID.size() < 32, "The payloadID is not high.");
									ImGui::SetDragDropPayload(payloadID.c_str(), &metadata.Handle, sizeof(metadata.Handle), ImGuiCond_Once);
									ImGui::EndDragDropSource();
								}
							}

							if (ImGui::TableSetColumnIndex(1)) {
								const auto typeStr = AssetTypeToString(metadata.Type);
								ImGui::Text(typeStr.c_str());
							}

							if (ImGui::TableSetColumnIndex(2)) {
								if (metadata.FilePath.source == FileSource::External) {
									const std::string nicePathStr = metadata.FilePath.path.string();
									ImGui::Text(nicePathStr.c_str());
								}
								else {
									const std::string nicePathStr = FileSourceToString(metadata.FilePath.source) + "/" + metadata.FilePath.path.string();
									ImGui::Text(nicePathStr.c_str());
								}
							}
							if (ImGui::TableSetColumnIndex(3)) {
								bool loaded = manager->m_LoadedAssets.contains(id);
								std::string label = "##" + id.string();
								if (ImGui::Checkbox(label.c_str(), &loaded)) {
									if (loaded) manager->LoadAsset(id);
									else manager->UnloadAsset(id);
								}
							}
						}

						ImGui::EndTable();
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Loaded Assets")) {
					static AssetHandle selected = NULL_ASSET_HANDLE;
					if (ImGui::BeginTable("LoadedAssetsTable", 2, flags)) {
						// Display headers so we can inspect their interaction with borders
						// (Headers are not the main purpose of this section of the demo, so we are not elaborating on them now. See other sections for details)
						{
							ImGui::TableSetupColumn("Handle");
							ImGui::TableSetupColumn("Type");
							ImGui::TableHeadersRow();
						}

						for (auto &[handle, asset]: manager->m_LoadedAssets) {
							ImGui::TableNextRow();
							ImGui::BeginGroup();
							if (ImGui::TableSetColumnIndex(0)) {
								const auto idStr = handle.GetID().raw_string();
								if (ImGui::Selectable(idStr.c_str(), selected == handle)) selected = handle;
								if (ImGui::BeginDragDropSource()) {
									std::string payloadID = AssetTypeToPayloadID(asset->GetType());
									MGN_CORE_CASSERT(payloadID.size() < 32, "The payloadID is not high.");
									ImGui::SetDragDropPayload(payloadID.c_str(), &handle, sizeof(handle), ImGuiCond_Once);
									ImGui::EndDragDropSource();
								}
							}

							if (ImGui::TableSetColumnIndex(1)) {
								const auto typeStr = AssetTypeToString(asset->GetType());
								ImGui::Text(typeStr.c_str());
							}

							ImGui::EndGroup();
						}

						ImGui::EndTable();
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Memory Assets")) {
					static AssetHandle selected = NULL_ASSET_HANDLE;
					if (ImGui::BeginTable("MemoryAssetsTable", 2, flags)) {
						// Display headers so we can inspect their interaction with borders
						// (Headers are not the main purpose of this section of the demo, so we are not elaborating on them now. See other sections for details)
						{
							ImGui::TableSetupColumn("Handle");
							ImGui::TableSetupColumn("Type");
							ImGui::TableHeadersRow();
						}

						for (auto &[handle, asset]: manager->m_MemoryAssets) {
							ImGui::TableNextRow();
							ImGui::BeginGroup();
							if (ImGui::TableSetColumnIndex(0)) {
								const auto idStr = handle.GetID().raw_string();
								if (ImGui::Selectable(idStr.c_str(), selected == handle)) selected = handle;
								if (ImGui::BeginDragDropSource()) {
									std::string payloadID = AssetTypeToPayloadID(asset->GetType());
									MGN_CORE_CASSERT(payloadID.size() < 32, "The payloadID is not high.");
									ImGui::SetDragDropPayload(payloadID.c_str(), &handle, sizeof(handle), ImGuiCond_Once);
									ImGui::EndDragDropSource();
								}
							}

							if (ImGui::TableSetColumnIndex(1)) {
								const auto typeStr = AssetTypeToString(asset->GetType());
								ImGui::Text(typeStr.c_str());
							}
							ImGui::EndGroup();
						}

						ImGui::EndTable();
					}

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}
		ImGui::End();
#endif
	}
} // namespace Imagine
