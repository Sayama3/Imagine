//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/ImGui/MaterialImGui.hpp"

#include "Imagine/Assets/Importers/MaterialSerializer.hpp"
#include "Imagine/Project/Project.hpp"
#ifdef MGN_IMGUI
#include <imgui.h>
#endif
namespace Imagine
{
#ifdef MGN_IMGUI
	bool MaterialImGui::OnMaterialImGui(Ref<Asset> asset)
	{
		if(asset->GetType() != CPUMaterial::GetStaticType()) return false;
		Ref<CPUMaterial> material = CastPtr<CPUMaterial>(asset);
		bool changed = false;

		// changed = material->OnImGui();

		if(ImGui::Button("Save")) {
			auto assetManager = Project::GetActive()->GetAssetManager();
			MaterialSerializer::ExportReadableMaterial(assetManager->GetMetadata(material->Handle), material);
		}
		return changed;
	}

	bool MaterialImGui::OnMenuMaterialImGui(AssetMetadata asset)
	{
		if(asset.Type != AssetType::Material) return false;
		bool shouldClose = false;

		// std::string name = "Set as default material##";
		// name += asset.Handle.string();
		// if(ImGui::MenuItem(name.c_str())) {
		// 	Project::SetDefaultMaterial(asset.Handle);
		// 	Project::SaveActive();
		// 	shouldClose = true;
		// }

//		name = "Set as gizmo material##";
//		name += asset.Handle.string();
//		if(ImGui::MenuItem(name.c_str())) {
//			Project::SetGizmoMaterial(asset.Handle);
//			Project::SaveActive();
//			shouldClose = true;
//		}

		return shouldClose;
	}
#else
	bool MaterialImGui::OnMaterialImGui(Ref<Asset> asset) { return false; }
	bool MaterialImGui::OnMenuMaterialImGui(AssetMetadata asset) { return false; }
#endif

} // namespace Imagine

