//
// Created by ianpo on 09/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUTexture2D.hpp"
#include "Imagine/Rendering/CPU/CPUTexture3D.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine
{
	class TextureImGui
	{
	public:
		static bool OnTexture2DImGui(Ref<Asset> asset);
		static bool OnTexture3DImGui(Ref<Asset> asset);
		static bool OnCubemapImGui(Ref<Asset> asset);
	};
}
