//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
// #include "Imagine/Rendering/CPU/CPUModel.hpp"

#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine
{
	class MeshImGui
	{
	public:
		static bool OnMeshImGui(Ref<Asset> asset);

		static bool OnModelImGui(Ref<Asset> asset);
	};
} // namespace Imagine

