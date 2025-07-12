//
// Created by ianpo on 11/07/2025.
//

#include "Imagine/Layers/ProjectLayer.hpp"
#include "Imagine/Assets/AssetManager.hpp"
#include "Imagine/Assets/FileAssetManager.hpp"
#include "Imagine/Project/Project.hpp"
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
			// project->m_Config
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
					if (ImGui::BeginTable("AssetRegistryTable", 3, flags)) {
						// Display headers so we can inspect their interaction with borders
						// (Headers are not the main purpose of this section of the demo, so we are not elaborating on them now. See other sections for details)
						{
							ImGui::TableSetupColumn("Handle");
							ImGui::TableSetupColumn("Type");
							ImGui::TableSetupColumn("Path");
							ImGui::TableHeadersRow();
						}

						for (auto &[id, metadata]: manager->m_AssetRegistry) {
							ImGui::TableNextRow();
							ImGui::BeginGroup();
							if (ImGui::TableSetColumnIndex(0)) {
								const auto idStr = metadata.Handle.GetID().raw_string();
								if (manager->m_LoadedAssets.contains(metadata.Handle)) {
									if (ImGui::Selectable(idStr.c_str(), selected == metadata.Handle)) selected = metadata.Handle;
									if (/*selected == metadata.Handle &&*/ ImGui::BeginDragDropSource()) {
										std::string payloadID = AssetTypeToPayloadID(metadata.Type);
										MGN_CORE_CASSERT(payloadID.size() < 32, "The payloadID is not high.");
										ImGui::SetDragDropPayload(payloadID.c_str(), &metadata.Handle, sizeof(metadata.Handle), ImGuiCond_Once);
										ImGui::EndDragDropSource();
									}
								} else {
									ImGui::Text(idStr.c_str());
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
							ImGui::EndGroup();
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
								if (selected == handle && ImGui::BeginDragDropSource()) {
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
								if (selected == handle && ImGui::BeginDragDropSource()) {
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
