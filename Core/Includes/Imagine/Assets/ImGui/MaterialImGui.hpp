//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include "Imagine/Rendering/MgnImGui.hpp"

namespace Imagine::Core
{

	class MaterialImGui
	{
	public:
		static bool OnMaterialImGui(Ref<Asset> asset);
		static bool OnMenuMaterialImGui(AssetMetadata asset);
	};

} // namespace Imagine::Core

