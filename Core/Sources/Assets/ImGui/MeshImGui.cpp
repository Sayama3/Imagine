//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/ImGui/MeshImGui.hpp"
#include "Imagine/Assets/Importers/MeshImporter.hpp"
#include "Imagine/Rendering/CPU/CPUShader.hpp"

namespace Imagine
{
	bool MeshImGui::OnMeshImGui(Ref<Asset> asset)
	{
		return false;
	}

	bool MeshImGui::OnModelImGui(Ref<Asset> asset)
	{
		return false;
	}
} // namespace Imagine

