//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine
{

	class SceneImGui
	{
	public:
		static bool OnSceneImGui(Ref<Asset> asset);
	};

} // namespace Imagine

